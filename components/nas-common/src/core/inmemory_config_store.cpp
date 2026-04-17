#include "nas/core/inmemory_config_store.hpp"

#include <cctype>
#include <sstream>

namespace nas::core {

// Minimal flat JSON parser: {"key": "value", ...}
Result<void> InMemoryConfigStore::LoadJson(const std::string& json) {
  std::map<std::string, std::string> parsed;
  std::size_t i = 0;
  const std::size_t n = json.size();

  auto skip_ws = [&]() {
    while (i < n && std::isspace(static_cast<unsigned char>(json[i]))) ++i;
  };

  auto parse_string = [&](std::string& out) -> bool {
    skip_ws();
    if (i >= n || json[i] != '"') return false;
    ++i;
    out.clear();
    while (i < n && json[i] != '"') {
      if (json[i] == '\\' && i + 1 < n) {
        ++i;
        switch (json[i]) {
          case '"':  out += '"';  break;
          case '\\': out += '\\'; break;
          case '/':  out += '/';  break;
          case 'n':  out += '\n'; break;
          case 'r':  out += '\r'; break;
          case 't':  out += '\t'; break;
          default:   out += json[i]; break;
        }
      } else {
        out += json[i];
      }
      ++i;
    }
    if (i >= n) return false;
    ++i;  // consume closing '"'
    return true;
  };

  skip_ws();
  if (i >= n || json[i] != '{') {
    return Fail(ErrorCode::kInvalidArgument, "expected '{'");
  }
  ++i;

  while (true) {
    skip_ws();
    if (i >= n) break;
    if (json[i] == '}') { ++i; break; }
    if (json[i] == ',') { ++i; continue; }

    std::string key;
    if (!parse_string(key)) {
      return Fail(ErrorCode::kInvalidArgument, "expected key string");
    }
    skip_ws();
    if (i >= n || json[i] != ':') {
      return Fail(ErrorCode::kInvalidArgument, "expected ':'");
    }
    ++i;
    std::string value;
    if (!parse_string(value)) {
      return Fail(ErrorCode::kInvalidArgument, "expected value string");
    }
    parsed[key] = value;
  }

  std::lock_guard<std::mutex> lock(mu_);
  data_ = std::move(parsed);
  return Ok();
}

// Minimal YAML parser: "key: value" lines (no nesting)
Result<void> InMemoryConfigStore::LoadYaml(const std::string& yaml) {
  std::map<std::string, std::string> parsed;
  std::istringstream ss(yaml);
  std::string line;

  auto trim = [](const std::string& s) -> std::string {
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return {};
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
  };

  while (std::getline(ss, line)) {
    const auto colon = line.find(':');
    if (colon == std::string::npos) continue;
    const auto key = trim(line.substr(0, colon));
    const auto value = trim(line.substr(colon + 1));
    if (!key.empty()) {
      parsed[key] = value;
    }
  }

  std::lock_guard<std::mutex> lock(mu_);
  data_ = std::move(parsed);
  return Ok();
}

Result<std::string> InMemoryConfigStore::DumpJson() const {
  auto escape = [](const std::string& s) -> std::string {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
      if (c == '"')       out += "\\\"";
      else if (c == '\\') out += "\\\\";
      else if (c == '\n') out += "\\n";
      else if (c == '\r') out += "\\r";
      else if (c == '\t') out += "\\t";
      else                out += c;
    }
    return out;
  };

  std::lock_guard<std::mutex> lock(mu_);
  std::string json = "{";
  bool first = true;
  for (const auto& [key, val] : data_) {
    if (!first) json += ",";
    first = false;
    json += "\"" + escape(key) + "\":\"" + escape(val) + "\"";
  }
  json += "}";
  return json;
}

Result<std::string> InMemoryConfigStore::Get(const std::string& key) const {
  std::lock_guard<std::mutex> lock(mu_);
  auto it = data_.find(key);
  if (it == data_.end()) {
    return Fail(ErrorCode::kNotFound, "key not found: " + key);
  }
  return it->second;
}

Result<void> InMemoryConfigStore::Set(const std::string& key, const std::string& value) {
  if (key.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "key is empty");
  }
  std::lock_guard<std::mutex> lock(mu_);
  data_[key] = value;
  return Ok();
}

}  // namespace nas::core
