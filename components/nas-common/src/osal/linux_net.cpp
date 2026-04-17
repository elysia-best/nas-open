#include "nas/osal/linux_net.hpp"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

namespace nas::osal {

Result<std::vector<NetworkInterface>> LinuxNetworkManager::ListInterfaces() {
  struct ifaddrs* ifap = nullptr;
  if (getifaddrs(&ifap) != 0) {
    return Fail(ErrorCode::kInternal, "getifaddrs failed");
  }

  std::vector<NetworkInterface> result;
  for (struct ifaddrs* ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == nullptr) continue;
    if (ifa->ifa_addr->sa_family != AF_INET) continue;

    NetworkInterface ni;
    ni.name = ifa->ifa_name;

    char addr_buf[INET_ADDRSTRLEN] = {};
    auto* sa = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
    inet_ntop(AF_INET, &sa->sin_addr, addr_buf, INET_ADDRSTRLEN);
    ni.address = addr_buf;

    if (ifa->ifa_netmask != nullptr) {
      char mask_buf[INET_ADDRSTRLEN] = {};
      auto* mask = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_netmask);
      inet_ntop(AF_INET, &mask->sin_addr, mask_buf, INET_ADDRSTRLEN);
      ni.netmask = mask_buf;
    }

    ni.up = (ifa->ifa_flags & IFF_UP) != 0;
    result.push_back(std::move(ni));
  }

  freeifaddrs(ifap);
  return result;
}

Result<void> LinuxNetworkManager::SetInterfaceUp(const std::string& name, bool up) {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    return Fail(ErrorCode::kInternal, "cannot create socket");
  }

  struct ifreq ifr = {};
  std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);

  if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
    close(fd);
    return Fail(ErrorCode::kNotFound, "interface not found: " + name);
  }

  if (up) {
    ifr.ifr_flags |= IFF_UP;
  } else {
    ifr.ifr_flags = static_cast<short>(ifr.ifr_flags & ~IFF_UP);
  }

  if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
    close(fd);
    return Fail(ErrorCode::kPermissionDenied, "cannot set interface flags: " + name);
  }

  close(fd);
  return Ok();
}

}  // namespace nas::osal
