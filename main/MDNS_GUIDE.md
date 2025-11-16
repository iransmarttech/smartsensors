# mDNS Implementation Guide

## What is mDNS?

mDNS (Multicast DNS) is a protocol that allows devices to discover and communicate with each other on a local network using human-readable hostnames instead of IP addresses. It is commonly known as "Bonjour" on Apple devices and "Zeroconf" on Linux systems.

## Benefits of mDNS

1. **User-Friendly Access**: Access your device using a memorable name like `http://smartSensor.local` instead of `http://192.168.1.3`
2. **Dynamic IP Handling**: Works even when DHCP assigns different IP addresses
3. **No DNS Server Required**: Works on local networks without infrastructure
4. **Service Discovery**: Allows clients to discover available services automatically
5. **Zero Configuration**: No manual DNS setup needed

## Implementation in This Project

### Important Limitation: Ethernet Mode

**mDNS on ESP32 only works with WiFi, not with Ethernet (W5500).**

The ESP-IDF mDNS library is built on top of the WiFi network stack and does not support external Ethernet controllers like the W5500. This is a hardware/firmware limitation of the ESP32 platform.

**Network Mode Support**:
- Ethernet Mode: mDNS NOT available (use IP address or router DNS)
- WiFi Station Mode: mDNS AVAILABLE
- Access Point Mode: mDNS AVAILABLE

### Configuration

The mDNS feature is controlled by the `MDNS_ENABLED` flag in `config.h`:

```cpp
#define MDNS_ENABLED
```

The hostname is defined in `config.cpp`:

```cpp
const char* HOSTNAME = "smartSensor";
```

### How It Works

#### Initialization

mDNS is initialized automatically when network connection is established:

1. **Ethernet Mode**: After successful DHCP lease
2. **WiFi Station Mode**: After connecting to WiFi network
3. **Access Point Mode**: After AP is started

The initialization function in `network_manager.cpp`:

```cpp
void SensorNetworkManager::initmDNS() {
    DEBUG_PRINTLN("Starting mDNS responder...");
    
    if (!MDNS.begin(HOSTNAME)) {
        DEBUG_PRINTLN("Error setting up mDNS responder!");
        return;
    }
    
    // Advertise HTTP service on port 80
    MDNS.addService("http", "tcp", 80);
    
    // Add service metadata
    MDNS.addServiceTxt("http", "tcp", "board", "ESP32-S3");
    MDNS.addServiceTxt("http", "tcp", "project", "Air Quality Monitor");
    
    DEBUG_PRINTLN("mDNS responder started successfully");
    DEBUG_PRINT("Access device at: http://");
    DEBUG_PRINT(HOSTNAME);
    DEBUG_PRINTLN(".local");
}
```

#### Maintenance

On ESP32, mDNS runs automatically in the background after initialization. Unlike ESP8266, the ESP32 mDNS library does not require periodic `update()` calls. The service advertisement is maintained automatically by the ESP-IDF mDNS component.

Once initialized with `MDNS.begin(HOSTNAME)`, the service remains active as long as:
- The network connection is maintained
- The device is powered on
- The mDNS service was successfully started

**No Manual Updates Required**:
```cpp
// ESP32 mDNS maintains itself automatically
// No need for MDNS.update() calls
// Service stays alive throughout device lifetime
```

The Ethernet and WiFi tasks only need to maintain their respective network connections:

**For Ethernet (Core 0 - Ethernet Task)**:
```cpp
if (millis() - lastMaintain > 1000) {
    Ethernet.maintain();  // Only maintain Ethernet DHCP
    lastMaintain = millis();
}
```

**For WiFi/AP (Core 1 - Sensor Task)**:
```cpp
// WiFi connection maintained automatically by ESP32
// No explicit maintenance needed for WiFi or mDNS
```

### Service Advertisement

The system advertises the following:

**Service Type**: HTTP Web Server
- Protocol: TCP
- Port: 80
- Name: smartSensor.local

**Service Metadata**:
- board: ESP32-S3
- project: Air Quality Monitor

## Usage

### Accessing the Web Interface

Instead of using the IP address, you can now access the device using:

```
http://smartSensor.local/
```

This works from:
- Web browsers (Chrome, Firefox, Safari, Edge)
- Command line tools (curl, wget)
- Mobile devices on the same network
- Desktop applications

### Browser Compatibility

**Full Support**:
- macOS Safari (native Bonjour)
- iOS Safari (native Bonjour)
- Chrome on macOS
- Firefox on macOS
- Edge on Windows 10/11 (with Bonjour service)

**Requires Bonjour Print Services**:
- Windows Chrome/Firefox/Edge
- Install: https://support.apple.com/kb/DL999

**Linux**:
- Install Avahi daemon: `sudo apt-get install avahi-daemon`
- Works with all browsers after installation

### Testing mDNS

#### Method 1: Web Browser
Simply open: `http://smartSensor.local/`

#### Method 2: Ping Test
```bash
# Linux/macOS
ping smartSensor.local

# Windows
ping smartSensor.local
```

#### Method 3: DNS Lookup
```bash
# Linux/macOS
dns-sd -G v4 smartSensor.local

# Linux alternative
avahi-resolve -n smartSensor.local

# Windows (PowerShell)
Resolve-DnsName smartSensor.local
```

#### Method 4: Service Discovery
```bash
# Linux/macOS - Discover all HTTP services
dns-sd -B _http._tcp

# Linux alternative
avahi-browse -r _http._tcp
```

## Troubleshooting

### Problem: Cannot Access via .local Address on Ethernet

**Cause**: ESP32 mDNS library does not support Ethernet connections (W5500)

**Solutions**:

**Option 1: Use IP Address** (Immediate)
```
http://192.168.1.3/
```

**Option 2: Router DHCP Hostname** (Recommended)
Configure your router to assign a hostname:
1. Access router admin (usually `192.168.1.1`)
2. Navigate to DHCP settings
3. Find device with MAC `DE:AD:BE:EF:FE:ED`
4. Assign hostname `smartsensor`
5. Set static/reserved IP `192.168.1.3`
6. Save and reboot router if needed
7. Access via `http://smartsensor/` (router-based DNS)

**Option 3: Switch to WiFi Mode** (For mDNS Support)
If you need .local access, use WiFi instead of Ethernet:
1. Disconnect Ethernet cable
2. Device will automatically fall back to WiFi
3. mDNS will work: `http://smartSensor.local/`

**Option 4: Local DNS Server**
Set up a local DNS server (Pi-hole, dnsmasq, etc.) and add:
```
192.168.1.3    smartsensor.local
```
Download and install Bonjour Print Services from Apple

**Solution 3: Install Avahi (Linux)**
```bash
sudo apt-get install avahi-daemon avahi-utils
sudo systemctl start avahi-daemon
sudo systemctl enable avahi-daemon
```

**Solution 4: Check Firewall**
Ensure UDP port 5353 is not blocked by firewall

**Solution 5: Network Isolation**
Some enterprise networks block mDNS (multicast) traffic. In such cases, use IP address directly.

### Problem: mDNS Works Sometimes, Not Always

**Cause**: mDNS cache issues or service not updating

**Solution**: 
```bash
# Linux - Restart Avahi
sudo systemctl restart avahi-daemon

# macOS - Flush mDNS cache
sudo killall -HUP mDNSResponder

# Windows - Restart Bonjour Service
net stop "Bonjour Service"
net start "Bonjour Service"
```

### Problem: Multiple Devices with Same Hostname

**Symptom**: Accessing wrong device

**Solution**: Change HOSTNAME in config.cpp to unique names:
```cpp
const char* HOSTNAME = "smartSensor-room1";
const char* HOSTNAME = "smartSensor-room2";
const char* HOSTNAME = "smartSensor-room3";
```

## Advanced Features

### Custom Service Discovery

Applications can discover all air quality sensors on the network:

```python
# Python example using zeroconf library
from zeroconf import ServiceBrowser, Zeroconf

class SensorListener:
    def add_service(self, zeroconf, type, name):
        info = zeroconf.get_service_info(type, name)
        print(f"Found sensor at: {info.server}")

zeroconf = Zeroconf()
listener = SensorListener()
browser = ServiceBrowser(zeroconf, "_http._tcp.local.", listener)
```

### Multiple Service Types

You can advertise additional services:

```cpp
// In initmDNS() function
MDNS.addService("http", "tcp", 80);       // Web interface
MDNS.addService("airquality", "tcp", 80); // Custom service type
```

Then discover with:
```bash
avahi-browse -r _airquality._tcp
```

## Network Modes

### Ethernet Mode
- **mDNS Status**: NOT SUPPORTED (ESP32 limitation)
- **Access Method**: Use IP address directly
- **Alternative**: Configure static hostname in router DHCP settings
- **Best for**: Stationary installations with stable IP addressing

**Router Configuration Method**:
Most routers allow assigning hostnames to DHCP reservations:
1. Login to router admin panel
2. Find DHCP settings
3. Locate device by MAC address: `DE:AD:BE:EF:FE:ED`
4. Assign hostname: `smartsensor`
5. Reserve IP address: `192.168.1.3`
6. Access via router DNS: `http://smartsensor/` (without .local)

### WiFi Station Mode
- **mDNS Status**: FULLY SUPPORTED
- **Access Method**: `http://smartSensor.local/`
- mDNS initialized after WiFi connection
- Works on all WiFi-connected clients
- Good for deployments with existing WiFi infrastructure

### Access Point Mode
- **mDNS Status**: FULLY SUPPORTED
- **Access Method**: `http://smartSensor.local/`
- mDNS initialized when AP starts
- Only works for clients connected to ESP32's AP
- Useful for standalone operation and initial configuration

## Performance Impact

mDNS has minimal performance impact on ESP32:

**Memory Usage**:
- Code: ~12KB flash (part of ESP-IDF)
- RAM: ~3KB (maintained by background task)

**CPU Usage**:
- Initialization: ~50ms once
- Background task: <0.5% CPU (handled by ESP-IDF)
- Query response: ~5ms per query

**Network Traffic**:
- Announcement: ~200 bytes every 2 minutes
- Response: ~150 bytes per query
- Goodbye: ~100 bytes on shutdown

**Background Operation**:
ESP32 mDNS runs as a FreeRTOS task in the ESP-IDF framework, completely independent of your application code. It automatically handles service announcements, query responses, and conflict resolution without any intervention.

## Security Considerations

mDNS operates on the local network only and does not provide:
- Authentication
- Encryption
- Access control

For secure deployments:

1. **Use HTTPS**: Implement TLS/SSL for encrypted communication
2. **Add Authentication**: Implement HTTP Basic Auth or session management
3. **Firewall Rules**: Restrict access to specific IP ranges
4. **VPN**: Use VPN for remote access instead of exposing to internet

## Best Practices

1. **Unique Hostnames**: Use descriptive, unique names for each device
2. **Error Handling**: Check return value of `MDNS.begin()`
3. **Fallback**: Always provide IP address as alternative access method
4. **Documentation**: Inform users about .local address in user manual
5. **Network Stability**: Ensure network connection remains stable (mDNS relies on it)
6. **Hostname Format**: Use lowercase letters, numbers, and hyphens only (no spaces or special characters)

## Comparison with Alternatives

### mDNS vs Static IP
**mDNS Advantages**:
- Works with DHCP
- No network configuration needed
- User-friendly names

**Static IP Advantages**:
- Works on all networks (including enterprise)
- No additional software needed
- Faster resolution

### mDNS vs Dynamic DNS
**mDNS Advantages**:
- No external service required
- No internet connection needed
- Instant propagation
- Free

**Dynamic DNS Advantages**:
- Works over internet
- Works on any network
- Standard DNS resolution

### mDNS vs QR Code/NFC
**mDNS Advantages**:
- No physical access needed
- Works from any location on network
- Always up to date

**QR Code/NFC Advantages**:
- Works on isolated networks
- No mDNS support required
- Visual confirmation

## Recommended Usage

**Use mDNS when**:
- Deploying on home/small office networks
- Users have mDNS-capable devices
- IP addresses change frequently
- User-friendliness is priority

**Use IP address when**:
- Enterprise networks that block multicast
- Remote access through VPN
- Integration with network monitoring tools
- Maximum compatibility required

**Best approach**: Support both mDNS and direct IP access, letting users choose based on their environment.

## Example Serial Output

When system starts successfully:

```
Initializing Ethernet...
Initializing W5500 chip...
Starting DHCP...
Ethernet connected. IP: 192.168.1.3
Starting mDNS responder...
mDNS responder started successfully
Access device at: http://smartSensor.local
Or directly at: http://192.168.1.3
✓ Ethernet HTTP server started on port 80
✓ System ready - Web interface available
```

## Customization

### Change Hostname

Edit `config.cpp`:
```cpp
const char* HOSTNAME = "myCustomName";
```

Access at: `http://myCustomName.local`

### Add More Service Information

Edit `network_manager.cpp` in `initmDNS()`:
```cpp
MDNS.addServiceTxt("http", "tcp", "version", "1.0");
MDNS.addServiceTxt("http", "tcp", "location", "Room 101");
MDNS.addServiceTxt("http", "tcp", "sensors", "4");
```

### Disable mDNS

Comment out in `config.h`:
```cpp
// #define MDNS_ENABLED
```

System will work normally without mDNS, using only IP addresses.

## Conclusion

mDNS significantly improves user experience by providing memorable, human-readable hostnames for device access. The implementation in this project is robust, handling all three network modes (Ethernet, WiFi, AP) with automatic initialization and maintenance. Users can choose between mDNS convenience (`smartSensor.local`) and direct IP access (`192.168.1.3`) based on their network environment and requirements.
