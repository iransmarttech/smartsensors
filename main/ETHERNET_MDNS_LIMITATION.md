# mDNS and Ethernet Limitation on ESP32

## The Problem

You cannot access the device using `http://smartSensor.local/` when using Ethernet, but the IP address `http://192.168.1.3/` works fine.

## Why This Happens

**ESP32 mDNS only works with WiFi, not with Ethernet controllers like the W5500.**

This is a fundamental limitation of the ESP-IDF (ESP32 framework):
- The mDNS library is built on top of the WiFi network stack
- External Ethernet controllers (W5500, ENC28J60, etc.) use a separate network interface
- ESP32 mDNS cannot bind to SPI-based Ethernet interfaces
- This affects all ESP32 models (ESP32, ESP32-S2, ESP32-S3, ESP32-C3)

## Your Options

### Option 1: Use IP Address (Current Solution)

**Access the device directly via IP**:
```
http://192.168.1.3/
```

**Pros**:
- Works immediately
- No configuration needed
- Most reliable

**Cons**:
- Must remember IP address
- If DHCP assigns different IP, must update bookmarks

**How to make IP permanent**:
Configure DHCP reservation in your router:
1. Login to router (usually `192.168.1.1`)
2. DHCP Settings > DHCP Reservations
3. Find MAC: `DE:AD:BE:EF:FE:ED`
4. Reserve IP: `192.168.1.3`
5. Save settings

### Option 2: Router DNS Configuration (Recommended)

**Configure hostname in your router's DHCP settings**:

Most modern routers allow assigning hostnames to devices:

1. **Access Router**: Navigate to `http://192.168.1.1` (or your router's IP)
2. **Login**: Use admin credentials
3. **Find DHCP Settings**: Usually under LAN/Network/DHCP section
4. **Locate Device**: Find the device with:
   - MAC Address: `DE:AD:BE:EF:FE:ED`
   - Current IP: `192.168.1.3`
5. **Set Hostname**: Assign name like `smartsensor` or `airquality`
6. **Reserve IP**: Enable static/reserved IP for this device
7. **Save and Apply**: Reboot router if prompted

**After configuration**, access via:
```
http://smartsensor/
```
(No `.local` suffix - this is router DNS, not mDNS)

**Pros**:
- Works with Ethernet
- User-friendly hostname
- Centralized management

**Cons**:
- Requires router configuration
- Only works on local network
- May not work on all router models

### Option 3: Switch to WiFi (For mDNS Support)

If you specifically need `.local` addresses:

1. **Disconnect Ethernet cable**
2. **Device automatically falls back to WiFi**
3. **Access via**: `http://smartSensor.local/`

**To force WiFi instead of Ethernet**:

Edit `config.h`:
```cpp
// #define ETHERNET_ENABLED    // Comment this out
#define WIFI_FALLBACK_ENABLED
```

**Pros**:
- Full mDNS support
- Works with `.local` addresses
- No router configuration needed

**Cons**:
- WiFi less reliable than Ethernet
- More power consumption
- Potential interference issues

### Option 4: Local DNS Server

Set up a DNS server on your network (advanced):

**Using Pi-hole, dnsmasq, or similar**:

Add to DNS configuration:
```
192.168.1.3    smartsensor.local
```

**Pros**:
- Works with Ethernet
- Can use `.local` suffix
- Centralized for all devices

**Cons**:
- Requires separate DNS server
- More complex setup
- Maintenance overhead

### Option 5: Browser Bookmark

**Simplest solution**:

1. Open `http://192.168.1.3/`
2. Bookmark it
3. Rename bookmark to "Smart Sensor" or "Air Quality Monitor"
4. Access via bookmark

**Pros**:
- No configuration needed
- Works on any device
- Easy to use

**Cons**:
- Not really a "hostname"
- Must create bookmark on each device

## Recommended Approach

**For Home/Small Office**:
Use **Option 2 (Router DNS)** if your router supports it, or **Option 5 (Bookmark)** for simplicity.

**For Industrial/Enterprise**:
Use **Option 1 (IP Address)** with DHCP reservation - most reliable and professional.

**If mDNS is Required**:
Use **Option 3 (WiFi)** - only way to get true mDNS support.

## Why Not Just Fix mDNS for Ethernet?

This would require:
1. Rewriting the ESP-IDF mDNS library to support multiple interfaces
2. Modifying the W5500 Ethernet library to integrate with mDNS
3. Changes to low-level network stack implementation
4. Extensive testing and debugging

This is beyond the scope of application-level code and would require contributions to ESP-IDF core libraries.

## Summary

| Solution | Works with Ethernet? | Uses .local? | Complexity | Recommended |
|----------|---------------------|--------------|------------|-------------|
| IP Address | Yes | No | Low | Yes |
| Router DNS | Yes | No | Medium | Yes |
| Switch to WiFi | No (WiFi only) | Yes | Low | If mDNS needed |
| Local DNS Server | Yes | Yes | High | Advanced only |
| Bookmark | Yes | No | Very Low | Simple solution |

## Current Implementation

The code now:
- Detects when running on Ethernet
- Skips mDNS initialization with informative message
- mDNS only enabled for WiFi/AP modes
- Clear serial output explaining the limitation

This is correct behavior and not a bug. Use IP address or configure your router for hostname-based access.
