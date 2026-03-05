/// mDNS service discovery using the bonsoir package.
/// Finds ESPHome devices advertising _gensetctrl._tcp on the local network.
library;

import 'dart:async';
import 'dart:io';
import 'package:bonsoir/bonsoir.dart';
import 'package:flutter/foundation.dart';

class DiscoveredDevice {
  final String name;
  final String host;
  final int port;

  DiscoveredDevice({required this.name, required this.host, required this.port});

  @override
  String toString() => '$name ($host:$port)';

  @override
  bool operator ==(Object other) =>
      other is DiscoveredDevice && other.host == host && other.port == port;

  @override
  int get hashCode => host.hashCode ^ port.hashCode;
}

class DiscoveryService extends ChangeNotifier {
  final List<DiscoveredDevice> _devices = [];
  final List<BonsoirDiscovery> _discoveries = [];
  bool _isScanning = false;
  bool _disposed = false;

  // Service types to scan — custom type + ESPHome default
  static const _serviceTypes = [
    '_gensetctrl._tcp',
    '_esphomelib._tcp',
  ];

  List<DiscoveredDevice> get devices => List.unmodifiable(_devices);
  bool get isScanning => _isScanning;

  /// Try to resolve a .local hostname to an IP via the system resolver.
  /// On Android 12+ this routes through the mDNS resolver.
  Future<String> _resolveHost(String hostname) async {
    if (!hostname.endsWith('.local')) return hostname;
    try {
      final results = await InternetAddress.lookup(hostname)
          .timeout(const Duration(seconds: 3));
      for (final addr in results) {
        if (addr.type == InternetAddressType.IPv4) {
          debugPrint('[mDNS] Resolved $hostname → ${addr.address}');
          return addr.address;
        }
      }
    } catch (e) {
      debugPrint('[mDNS] System resolve failed for $hostname: $e');
    }
    return hostname;
  }

  void _addDevice(String name, String host, int port) {
    if (_disposed) return;
    final device = DiscoveredDevice(name: name, host: host, port: port);
    if (!_devices.contains(device)) {
      _devices.add(device);
      notifyListeners();
    }
  }

  Future<void> startScan() async {
    if (_isScanning || _disposed) return;
    _devices.clear();
    _isScanning = true;
    notifyListeners();

    for (final type in _serviceTypes) {
      try {
        debugPrint('[mDNS] Starting scan for $type');
        final discovery = BonsoirDiscovery(type: type);
        await discovery.ready;

        discovery.eventStream!.listen((event) {
          debugPrint('[mDNS] Event: ${event.type} for $type — service: ${event.service?.name}');
          if (event.type == BonsoirDiscoveryEventType.discoveryServiceResolved) {
            final service = event.service as ResolvedBonsoirService;
            final host = service.host ?? service.name;
            final port = service.port;
            debugPrint('[mDNS] Resolved: ${service.name} → $host:$port');

            _addDevice(service.name, host, port > 0 ? port : 80);
          } else if (event.type == BonsoirDiscoveryEventType.discoveryServiceFound) {
            // On Android, resolved event may never fire.
            // Attempt to resolve the IP ourselves via system mDNS.
            final service = event.service;
            if (service != null) {
              final name = service.name;
              final port = service.port > 0 ? service.port : 80;
              debugPrint('[mDNS] Found (unresolved): $name — resolving IP...');
              _resolveHost('$name.local').then((host) {
                debugPrint('[mDNS] Using host: $host for $name');
                _addDevice(name, host, port);
              });
            }
          } else if (event.type == BonsoirDiscoveryEventType.discoveryServiceLost) {
            final service = event.service;
            if (service != null && !_disposed) {
              _devices.removeWhere(
                  (d) => d.name == service.name && d.port == service.port);
              if (!_disposed) notifyListeners();
            }
          }
        });

        await discovery.start();
        _discoveries.add(discovery);
      } catch (e) {
        debugPrint('mDNS scan failed for $type: $e');
      }
    }

    // Auto-stop after 15 seconds
    Future.delayed(const Duration(seconds: 15), () {
      if (_isScanning && !_disposed) stopScan();
    });
  }

  Future<void> stopScan() async {
    if (!_isScanning || _disposed) return;
    for (final d in _discoveries) {
      try { await d.stop(); } catch (_) {}
    }
    _discoveries.clear();
    _isScanning = false;
    if (!_disposed) notifyListeners();
  }

  @override
  void dispose() {
    _disposed = true;
    for (final d in _discoveries) {
      try { d.stop(); } catch (_) {}
    }
    _discoveries.clear();
    _isScanning = false;
    super.dispose();
  }
}
