/// Discovery screen — finds ESPHome devices via mDNS and allows manual entry.
library;

import 'dart:io';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:shared_preferences/shared_preferences.dart';
import '../services/discovery_service.dart';
import '../services/genset_provider.dart';
import '../theme/app_theme.dart';
import 'home_screen.dart';

class DiscoveryScreen extends StatefulWidget {
  const DiscoveryScreen({super.key});

  @override
  State<DiscoveryScreen> createState() => _DiscoveryScreenState();
}

class _DiscoveryScreenState extends State<DiscoveryScreen> {
  final _discovery = DiscoveryService();
  final _hostController = TextEditingController();
  final _portController = TextEditingController(text: '80');
  bool _autoConnecting = false;

  @override
  void initState() {
    super.initState();
    _loadLastHost();
    _discovery.startScan();
  }

  Future<void> _loadLastHost() async {
    final prefs = await SharedPreferences.getInstance();
    final lastHost = prefs.getString('last_host');
    final lastPort = prefs.getInt('last_port') ?? 80;
    if (lastHost != null && lastHost.isNotEmpty) {
      _hostController.text = lastHost;
      _portController.text = lastPort.toString();
      // Auto-connect to last known host
      _autoConnect(lastHost, lastPort);
    }
  }

  Future<void> _autoConnect(String host, int port) async {
    setState(() => _autoConnecting = true);
    // Resolve .local hostname to IP before connecting
    final resolved = await _resolveLocal(host);
    if (resolved != host) {
      // Save the resolved IP so next launch connects directly
      final prefs = await SharedPreferences.getInstance();
      await prefs.setString('last_host', resolved);
    }
    final provider = context.read<GensetProvider>();
    provider.connect(resolved, port: port);

    // Wait up to 5 seconds for first successful poll
    for (int i = 0; i < 10; i++) {
      await Future.delayed(const Duration(milliseconds: 500));
      if (provider.connected) {
        if (mounted) _navigateToDashboard();
        return;
      }
    }
    if (mounted) {
      setState(() => _autoConnecting = false);
      provider.disconnect();
    }
  }

  Future<void> _saveAndConnect(String host, int port) async {
    // Resolve .local hostname to IP before connecting/saving
    final resolved = await _resolveLocal(host);
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('last_host', resolved);
    await prefs.setInt('last_port', port);

    if (!mounted) return;
    context.read<GensetProvider>().connect(resolved, port: port);
    _navigateToDashboard();
  }

  /// Attempt to resolve a .local hostname to a real IP address.
  Future<String> _resolveLocal(String host) async {
    if (!host.endsWith('.local')) return host;
    try {
      final results = await InternetAddress.lookup(host)
          .timeout(const Duration(seconds: 3));
      for (final addr in results) {
        if (addr.type == InternetAddressType.IPv4) {
          debugPrint('[connect] Resolved $host → ${addr.address}');
          return addr.address;
        }
      }
    } catch (e) {
      debugPrint('[connect] Failed to resolve $host: $e');
    }
    return host;
  }

  void _navigateToDashboard() {
    Navigator.of(context).pushReplacement(
      MaterialPageRoute(builder: (_) => const HomeScreen()),
    );
  }

  @override
  void dispose() {
    _discovery.dispose();
    _hostController.dispose();
    _portController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: _autoConnecting ? _buildAutoConnecting() : _buildDiscovery(),
      ),
    );
  }

  Widget _buildAutoConnecting() {
    return Center(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          const SizedBox(
            width: 48,
            height: 48,
            child: CircularProgressIndicator(strokeWidth: 3),
          ),
          const SizedBox(height: 20),
          Text(
            'Connecting to ${_hostController.text}...',
            style: Theme.of(context).textTheme.bodyLarge,
          ),
          const SizedBox(height: 24),
          TextButton(
            onPressed: () => setState(() => _autoConnecting = false),
            child: const Text('Cancel'),
          ),
        ],
      ),
    );
  }

  Widget _buildDiscovery() {
    return Padding(
      padding: const EdgeInsets.all(20),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const SizedBox(height: 40),
          // Title
          Row(
            children: [
              Icon(Icons.bolt, color: AppTheme.accent, size: 32),
              const SizedBox(width: 10),
              Text(
                'GenSet Controller',
                style: Theme.of(context).textTheme.headlineLarge,
              ),
            ],
          ),
          const SizedBox(height: 6),
          Text(
            'SmartGen HSC941 Monitor & Control',
            style: Theme.of(context).textTheme.bodySmall,
          ),
          const SizedBox(height: 30),

          // Manual entry
          Text('Connect manually',
              style: Theme.of(context).textTheme.titleMedium),
          const SizedBox(height: 10),
          Row(
            children: [
              Expanded(
                flex: 3,
                child: TextField(
                  controller: _hostController,
                  decoration: InputDecoration(
                    hintText: 'IP address or hostname',
                    filled: true,
                    fillColor: AppTheme.surface,
                    border: OutlineInputBorder(
                      borderRadius: BorderRadius.circular(8),
                      borderSide: const BorderSide(color: AppTheme.border),
                    ),
                    enabledBorder: OutlineInputBorder(
                      borderRadius: BorderRadius.circular(8),
                      borderSide: const BorderSide(color: AppTheme.border),
                    ),
                    contentPadding: const EdgeInsets.symmetric(
                        horizontal: 14, vertical: 12),
                  ),
                  style: const TextStyle(fontSize: 14),
                ),
              ),
              const SizedBox(width: 8),
              SizedBox(
                width: 72,
                child: TextField(
                  controller: _portController,
                  keyboardType: TextInputType.number,
                  decoration: InputDecoration(
                    hintText: 'Port',
                    filled: true,
                    fillColor: AppTheme.surface,
                    border: OutlineInputBorder(
                      borderRadius: BorderRadius.circular(8),
                      borderSide: const BorderSide(color: AppTheme.border),
                    ),
                    enabledBorder: OutlineInputBorder(
                      borderRadius: BorderRadius.circular(8),
                      borderSide: const BorderSide(color: AppTheme.border),
                    ),
                    contentPadding: const EdgeInsets.symmetric(
                        horizontal: 10, vertical: 12),
                  ),
                  style: const TextStyle(fontSize: 14),
                ),
              ),
              const SizedBox(width: 8),
              ElevatedButton(
                onPressed: () {
                  final host = _hostController.text.trim();
                  final port =
                      int.tryParse(_portController.text.trim()) ?? 80;
                  if (host.isNotEmpty) _saveAndConnect(host, port);
                },
                child: const Text('Connect'),
              ),
            ],
          ),

          const SizedBox(height: 30),
          const Divider(),
          const SizedBox(height: 16),

          // mDNS discovered devices
          Row(
            children: [
              Text('Discovered devices',
                  style: Theme.of(context).textTheme.titleMedium),
              const Spacer(),
              ListenableBuilder(
                listenable: _discovery,
                builder: (_, __) {
                  if (_discovery.isScanning) {
                    return const SizedBox(
                      width: 16,
                      height: 16,
                      child: CircularProgressIndicator(strokeWidth: 2),
                    );
                  }
                  return IconButton(
                    icon: const Icon(Icons.refresh, size: 20),
                    onPressed: () => _discovery.startScan(),
                    tooltip: 'Scan again',
                  );
                },
              ),
            ],
          ),
          const SizedBox(height: 10),
          Expanded(
            child: ListenableBuilder(
              listenable: _discovery,
              builder: (_, __) {
                final devices = _discovery.devices;
                if (devices.isEmpty && _discovery.isScanning) {
                  return Center(
                    child: Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        Icon(Icons.wifi_find,
                            size: 48, color: AppTheme.dim),
                        const SizedBox(height: 12),
                        Text('Scanning network...',
                            style: Theme.of(context).textTheme.bodySmall),
                      ],
                    ),
                  );
                }
                if (devices.isEmpty) {
                  return Center(
                    child: Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        Icon(Icons.devices_other,
                            size: 48, color: AppTheme.dim),
                        const SizedBox(height: 12),
                        Text('No devices found',
                            style: Theme.of(context).textTheme.bodySmall),
                        const SizedBox(height: 8),
                        Text(
                            'Make sure you\'re on the same network\nas the generator controller.',
                            textAlign: TextAlign.center,
                            style: TextStyle(
                                fontSize: 12, color: AppTheme.dim)),
                      ],
                    ),
                  );
                }
                return ListView.builder(
                  itemCount: devices.length,
                  itemBuilder: (context, index) {
                    final device = devices[index];
                    return Card(
                      child: ListTile(
                        leading: const Icon(Icons.developer_board,
                            color: AppTheme.accent),
                        title: Text(device.name,
                            style: const TextStyle(fontWeight: FontWeight.w600)),
                        subtitle: Text('${device.host}:${device.port}',
                            style: const TextStyle(
                                fontSize: 12, color: AppTheme.dim)),
                        trailing: const Icon(Icons.chevron_right),
                        onTap: () {
                          _hostController.text = device.host;
                          _portController.text = device.port.toString();
                          _saveAndConnect(device.host, device.port);
                        },
                      ),
                    );
                  },
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}
