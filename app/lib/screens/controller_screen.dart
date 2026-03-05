/// Controller screen — read-only view of HSC941 registers 0x0042–0x00E9.
/// Mirrors the web dashboard's "Controller" tab with grouped parameters.
library;

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../services/genset_provider.dart';
import '../models/config_models.dart';
import '../theme/app_theme.dart';

/// Single parameter definition.
class _Param {
  final int addr;
  final String name;
  final double scale;
  final String unit;
  final String group;

  const _Param(this.addr, this.name, this.scale, this.unit, this.group);
}

// Matches the web JS CFG_MAP — subset of most useful registers.
const _cfgMap = <_Param>[
  // Timing
  _Param(0x42, 'Start Delay', 1, 's', 'Timing'),
  _Param(0x43, 'Stop Delay', 1, 's', 'Timing'),
  _Param(0x44, 'Warm-up Time', 1, 's', 'Timing'),
  _Param(0x45, 'Cooling Time', 1, 's', 'Timing'),
  _Param(0x46, 'Safety-on Delay', 1, 's', 'Timing'),
  _Param(0x47, 'Idle Time', 1, 's', 'Timing'),
  _Param(0x48, 'Return Delay', 1, 's', 'Timing'),
  _Param(0x49, 'Crank Time', 1, 's', 'Timing'),
  _Param(0x4A, 'Crank Rest', 1, 's', 'Timing'),
  _Param(0x4B, 'Crank Attempts', 1, '', 'Timing'),

  // Electrical Ratings
  _Param(0x4E, 'Rated Voltage', 1, 'V', 'Electrical'),
  _Param(0x4F, 'Rated Current', 1, 'A', 'Electrical'),
  _Param(0x50, 'Rated Frequency', 0.1, 'Hz', 'Electrical'),
  _Param(0x51, 'Rated Speed', 1, 'RPM', 'Electrical'),
  _Param(0x52, 'CT Ratio', 1, '', 'Electrical'),
  _Param(0x53, 'Phase Config', 1, '', 'Electrical'),

  // Protection
  _Param(0x56, 'Over-voltage Threshold', 1, 'V', 'Protection'),
  _Param(0x57, 'Under-voltage Threshold', 1, 'V', 'Protection'),
  _Param(0x58, 'Over-frequency', 0.1, 'Hz', 'Protection'),
  _Param(0x59, 'Under-frequency', 0.1, 'Hz', 'Protection'),
  _Param(0x5A, 'Over-current Threshold', 1, 'A', 'Protection'),
  _Param(0x5B, 'Over-current Delay', 1, 's', 'Protection'),
  _Param(0x5E, 'Reverse Power Threshold', 0.1, '%', 'Protection'),
  _Param(0x5F, 'Reverse Power Delay', 1, 's', 'Protection'),

  // Sensor Thresholds
  _Param(0x62, 'High Water Temp Alarm', 1, '°C', 'Sensor Thresholds'),
  _Param(0x63, 'High Water Temp Shutdown', 1, '°C', 'Sensor Thresholds'),
  _Param(0x64, 'Low Oil Pressure Alarm', 1, 'kPa', 'Sensor Thresholds'),
  _Param(0x65, 'Low Oil Pressure Shutdown', 1, 'kPa', 'Sensor Thresholds'),
  _Param(0x66, 'Over-speed Threshold', 1, 'RPM', 'Sensor Thresholds'),
  _Param(0x67, 'Under-speed Threshold', 1, 'RPM', 'Sensor Thresholds'),

  // Battery
  _Param(0x6C, 'Battery Under-voltage', 0.1, 'V', 'Battery'),
  _Param(0x6D, 'Battery Over-voltage', 0.1, 'V', 'Battery'),
  _Param(0x6E, 'Charge Over-voltage', 0.1, 'V', 'Battery'),
  _Param(0x6F, 'Pre-heat Time', 1, 's', 'Battery'),

  // Speed / Governor
  _Param(0x72, 'Speed Source', 1, '', 'Governor'),
  _Param(0x73, 'Governor P', 0.1, '', 'Governor'),
  _Param(0x74, 'Governor I', 0.1, '', 'Governor'),
  _Param(0x75, 'Governor D', 0.1, '', 'Governor'),

  // Mains thresholds
  _Param(0x80, 'Mains Over-voltage', 1, 'V', 'Mains'),
  _Param(0x81, 'Mains Under-voltage', 1, 'V', 'Mains'),
  _Param(0x82, 'Mains Over-frequency', 0.1, 'Hz', 'Mains'),
  _Param(0x83, 'Mains Under-frequency', 0.1, 'Hz', 'Mains'),
];

class ControllerScreen extends StatefulWidget {
  const ControllerScreen({super.key});

  @override
  State<ControllerScreen> createState() => _ControllerScreenState();
}

class _ControllerScreenState extends State<ControllerScreen> {
  ConfigData? _config;
  bool _loading = false;
  String? _error;

  Future<void> _loadConfig() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    final api = context.read<GensetProvider>().api;
    if (api == null) {
      setState(() {
        _error = 'Not connected';
        _loading = false;
      });
      return;
    }
    try {
      final cfg = await api.getConfig();
      if (mounted) {
        setState(() {
          _config = cfg;
          _loading = false;
        });
      }
    } catch (e) {
      if (mounted) {
        setState(() {
          _error = e.toString();
          _loading = false;
        });
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    if (_config == null && !_loading && _error == null) {
      return _buildPrompt();
    }
    if (_loading) {
      return const Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            CircularProgressIndicator(),
            SizedBox(height: 16),
            Text('Reading controller registers...',
                style: TextStyle(color: AppTheme.dim)),
            Text('This may take a few seconds',
                style: TextStyle(color: AppTheme.dim, fontSize: 12)),
          ],
        ),
      );
    }
    if (_error != null) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(Icons.error_outline, size: 48, color: AppTheme.red),
            const SizedBox(height: 12),
            Text(_error!, style: const TextStyle(color: AppTheme.dim)),
            const SizedBox(height: 12),
            ElevatedButton.icon(
              onPressed: _loadConfig,
              icon: const Icon(Icons.refresh, size: 16),
              label: const Text('Retry'),
            ),
          ],
        ),
      );
    }
    return _buildData();
  }

  Widget _buildPrompt() {
    return Center(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(Icons.memory, size: 56, color: AppTheme.dim),
          const SizedBox(height: 16),
          Text('Controller Parameters',
              style: Theme.of(context).textTheme.headlineMedium),
          const SizedBox(height: 8),
          const Text(
            'Read all configuration registers (0x0042–0x00E9)\nfrom the HSC941 controller via Modbus.',
            textAlign: TextAlign.center,
            style: TextStyle(color: AppTheme.dim, fontSize: 13),
          ),
          const SizedBox(height: 20),
          ElevatedButton.icon(
            onPressed: _loadConfig,
            icon: const Icon(Icons.download, size: 18),
            label: const Text('Read Configuration'),
          ),
        ],
      ),
    );
  }

  Widget _buildData() {
    // Group parameters
    final groups = <String, List<_Param>>{};
    for (final p in _cfgMap) {
      groups.putIfAbsent(p.group, () => []).add(p);
    }

    return RefreshIndicator(
      onRefresh: _loadConfig,
      child: ListView(
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
        children: [
          // Summary
          Padding(
            padding: const EdgeInsets.only(bottom: 12),
            child: Row(
              children: [
                const Icon(Icons.memory, size: 16, color: AppTheme.dim),
                const SizedBox(width: 6),
                Text(
                  '${_config!.count} registers read from 0x${_config!.start.toRadixString(16).toUpperCase().padLeft(4, '0')}',
                  style: const TextStyle(fontSize: 12, color: AppTheme.dim),
                ),
                const Spacer(),
                IconButton(
                  icon: const Icon(Icons.refresh, size: 18),
                  onPressed: _loadConfig,
                  tooltip: 'Re-read',
                ),
              ],
            ),
          ),
          ...groups.entries.map((entry) => _buildGroup(entry.key, entry.value)),
          const SizedBox(height: 20),
        ],
      ),
    );
  }

  Widget _buildGroup(String name, List<_Param> params) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(name,
                style: Theme.of(context).textTheme.titleMedium),
            const SizedBox(height: 8),
            ...params.map((p) {
              final raw = _config!.registerAt(p.addr);
              final formatted = raw != null
                  ? p.scale == 1
                      ? '$raw ${p.unit}'.trim()
                      : '${(raw * p.scale).toStringAsFixed(1)} ${p.unit}'
                          .trim()
                  : '—';
              return Padding(
                padding: const EdgeInsets.symmetric(vertical: 3),
                child: Row(
                  children: [
                    Expanded(
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          Text(p.name,
                              style: const TextStyle(fontSize: 12)),
                          Text(
                            '0x${p.addr.toRadixString(16).toUpperCase().padLeft(4, '0')}',
                            style: const TextStyle(
                                fontSize: 9,
                                color: AppTheme.dim,
                                fontFamily: 'monospace'),
                          ),
                        ],
                      ),
                    ),
                    Text(
                      formatted,
                      style: const TextStyle(
                        fontSize: 13,
                        fontWeight: FontWeight.w600,
                        fontFamily: 'monospace',
                      ),
                    ),
                  ],
                ),
              );
            }),
          ],
        ),
      ),
    );
  }
}
