/// History screen — event log, battery voltage chart, runtime bar chart.
library;

import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:provider/provider.dart';
import '../services/genset_provider.dart';
import '../models/config_models.dart';
import '../theme/app_theme.dart';

class HistoryScreen extends StatefulWidget {
  const HistoryScreen({super.key});

  @override
  State<HistoryScreen> createState() => _HistoryScreenState();
}

class _HistoryScreenState extends State<HistoryScreen>
    with SingleTickerProviderStateMixin {
  late final TabController _tabController;

  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: 3, vsync: this);
  }

  @override
  void dispose() {
    _tabController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        TabBar(
          controller: _tabController,
          labelColor: AppTheme.accent,
          unselectedLabelColor: AppTheme.dim,
          indicatorColor: AppTheme.accent,
          tabs: const [
            Tab(text: 'Events', icon: Icon(Icons.list_alt, size: 18)),
            Tab(text: 'Battery', icon: Icon(Icons.battery_std, size: 18)),
            Tab(text: 'Runtime', icon: Icon(Icons.bar_chart, size: 18)),
          ],
        ),
        Expanded(
          child: TabBarView(
            controller: _tabController,
            children: const [
              _EventLogTab(),
              _BatteryTab(),
              _RuntimeTab(),
            ],
          ),
        ),
      ],
    );
  }
}

// ── Event Log ──
class _EventLogTab extends StatefulWidget {
  const _EventLogTab();

  @override
  State<_EventLogTab> createState() => _EventLogTabState();
}

class _EventLogTabState extends State<_EventLogTab>
    with AutomaticKeepAliveClientMixin {
  List<EventEntry>? _events;
  bool _loading = true;
  String? _error;

  @override
  bool get wantKeepAlive => true;

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    final api = context.read<GensetProvider>().api;
    if (api == null) return;
    try {
      _events = await api.getEventLog();
    } catch (e) {
      _error = e.toString();
    }
    if (mounted) setState(() => _loading = false);
  }

  @override
  Widget build(BuildContext context) {
    super.build(context);
    if (_loading) return const Center(child: CircularProgressIndicator());
    if (_error != null) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(_error!, style: const TextStyle(color: AppTheme.dim)),
            const SizedBox(height: 8),
            ElevatedButton(onPressed: _load, child: const Text('Retry')),
          ],
        ),
      );
    }
    if (_events == null || _events!.isEmpty) {
      return const Center(
          child: Text('No events recorded',
              style: TextStyle(color: AppTheme.dim)));
    }
    return RefreshIndicator(
      onRefresh: _load,
      child: ListView.separated(
        padding: const EdgeInsets.all(12),
        itemCount: _events!.length,
        separatorBuilder: (_, __) => const Divider(height: 1),
        itemBuilder: (_, i) {
          final e = _events![i];
          final time =
              '${e.timestamp.month}/${e.timestamp.day} ${e.timestamp.hour.toString().padLeft(2, '0')}:${e.timestamp.minute.toString().padLeft(2, '0')}';
          final isErr = e.message.toLowerCase().contains('alarm') ||
              e.message.toLowerCase().contains('shutdown') ||
              e.message.toLowerCase().contains('fail');
          return ListTile(
            dense: true,
            contentPadding: EdgeInsets.zero,
            leading: Icon(
              isErr ? Icons.warning_amber : Icons.info_outline,
              size: 18,
              color: isErr ? AppTheme.red : AppTheme.dim,
            ),
            title:
                Text(e.message, style: const TextStyle(fontSize: 13)),
            trailing: Text(time,
                style: const TextStyle(
                    fontSize: 11,
                    color: AppTheme.dim,
                    fontFamily: 'monospace')),
          );
        },
      ),
    );
  }
}

// ── Battery Voltage Chart ──
class _BatteryTab extends StatefulWidget {
  const _BatteryTab();

  @override
  State<_BatteryTab> createState() => _BatteryTabState();
}

class _BatteryTabState extends State<_BatteryTab>
    with AutomaticKeepAliveClientMixin {
  BatteryHistory? _history;
  bool _loading = true;
  String? _error;

  @override
  bool get wantKeepAlive => true;

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    final api = context.read<GensetProvider>().api;
    if (api == null) return;
    try {
      _history = await api.getBatteryHistory();
    } catch (e) {
      _error = e.toString();
    }
    if (mounted) setState(() => _loading = false);
  }

  @override
  Widget build(BuildContext context) {
    super.build(context);
    if (_loading) return const Center(child: CircularProgressIndicator());
    if (_error != null) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(_error!, style: const TextStyle(color: AppTheme.dim)),
            const SizedBox(height: 8),
            ElevatedButton(onPressed: _load, child: const Text('Retry')),
          ],
        ),
      );
    }
    final h = _history;
    if (h == null || h.samples.isEmpty) {
      return const Center(
          child: Text('No battery data yet',
              style: TextStyle(color: AppTheme.dim)));
    }

    final spots = <FlSpot>[];
    for (int i = 0; i < h.samples.length; i++) {
      spots.add(FlSpot(i.toDouble(), h.samples[i]));
    }

    return Padding(
      padding: const EdgeInsets.all(16),
      child: Column(
        children: [
          // Stats row
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceAround,
            children: [
              _StatChip(label: 'Min', value: '${h.min24h.toStringAsFixed(2)} V'),
              _StatChip(
                  label: 'Avg', value: '${h.avg24h.toStringAsFixed(2)} V'),
              _StatChip(
                  label: 'Max', value: '${h.max24h.toStringAsFixed(2)} V'),
            ],
          ),
          const SizedBox(height: 16),
          // Chart
          Expanded(
            child: LineChart(
              LineChartData(
                gridData: FlGridData(
                  show: true,
                  drawVerticalLine: false,
                  horizontalInterval: 0.5,
                  getDrawingHorizontalLine: (value) => FlLine(
                    color: AppTheme.faint,
                    strokeWidth: 0.5,
                  ),
                ),
                titlesData: FlTitlesData(
                  leftTitles: AxisTitles(
                    sideTitles: SideTitles(
                      showTitles: true,
                      reservedSize: 36,
                      interval: 1,
                      getTitlesWidget: (v, _) => Text(
                        '${v.toStringAsFixed(0)}V',
                        style: const TextStyle(
                            fontSize: 10, color: AppTheme.dim),
                      ),
                    ),
                  ),
                  bottomTitles: const AxisTitles(
                      sideTitles: SideTitles(showTitles: false)),
                  topTitles: const AxisTitles(
                      sideTitles: SideTitles(showTitles: false)),
                  rightTitles: const AxisTitles(
                      sideTitles: SideTitles(showTitles: false)),
                ),
                borderData: FlBorderData(show: false),
                lineBarsData: [
                  LineChartBarData(
                    spots: spots,
                    isCurved: true,
                    curveSmoothness: 0.3,
                    color: AppTheme.cyan,
                    barWidth: 2,
                    dotData: const FlDotData(show: false),
                    belowBarData: BarAreaData(
                      show: true,
                      color: AppTheme.cyan.withAlpha(25),
                    ),
                  ),
                ],
                lineTouchData: LineTouchData(
                  touchTooltipData: LineTouchTooltipData(
                    getTooltipItems: (spots) => spots
                        .map((s) => LineTooltipItem(
                              '${s.y.toStringAsFixed(2)} V',
                              const TextStyle(
                                  color: AppTheme.cyan,
                                  fontSize: 12,
                                  fontWeight: FontWeight.w600),
                            ))
                        .toList(),
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

// ── Runtime Bar Chart ──
class _RuntimeTab extends StatefulWidget {
  const _RuntimeTab();

  @override
  State<_RuntimeTab> createState() => _RuntimeTabState();
}

class _RuntimeTabState extends State<_RuntimeTab>
    with AutomaticKeepAliveClientMixin {
  List<RuntimeDay>? _days;
  bool _loading = true;
  String? _error;

  @override
  bool get wantKeepAlive => true;

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    final api = context.read<GensetProvider>().api;
    if (api == null) return;
    try {
      _days = await api.getRuntimeHistory();
    } catch (e) {
      _error = e.toString();
    }
    if (mounted) setState(() => _loading = false);
  }

  @override
  Widget build(BuildContext context) {
    super.build(context);
    if (_loading) return const Center(child: CircularProgressIndicator());
    if (_error != null) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(_error!, style: const TextStyle(color: AppTheme.dim)),
            const SizedBox(height: 8),
            ElevatedButton(onPressed: _load, child: const Text('Retry')),
          ],
        ),
      );
    }
    if (_days == null || _days!.isEmpty) {
      return const Center(
          child: Text('No runtime data yet',
              style: TextStyle(color: AppTheme.dim)));
    }

    final maxHours =
        _days!.fold<double>(0, (m, d) => d.hours > m ? d.hours : m);
    final totalHours = _days!.fold<double>(0, (s, d) => s + d.hours);

    return Padding(
      padding: const EdgeInsets.all(16),
      child: Column(
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceAround,
            children: [
              _StatChip(
                  label: 'Total',
                  value: '${totalHours.toStringAsFixed(1)} h'),
              _StatChip(
                  label: 'Days', value: '${_days!.length}'),
              _StatChip(
                  label: 'Max/day',
                  value: '${maxHours.toStringAsFixed(1)} h'),
            ],
          ),
          const SizedBox(height: 16),
          Expanded(
            child: BarChart(
              BarChartData(
                alignment: BarChartAlignment.spaceAround,
                maxY: (maxHours * 1.2).ceilToDouble(),
                gridData: FlGridData(
                  show: true,
                  drawVerticalLine: false,
                  horizontalInterval:
                      maxHours > 4 ? (maxHours / 4).ceilToDouble() : 1,
                  getDrawingHorizontalLine: (value) => FlLine(
                    color: AppTheme.faint,
                    strokeWidth: 0.5,
                  ),
                ),
                titlesData: FlTitlesData(
                  leftTitles: AxisTitles(
                    sideTitles: SideTitles(
                      showTitles: true,
                      reservedSize: 30,
                      getTitlesWidget: (v, _) => Text(
                        '${v.toStringAsFixed(0)}h',
                        style: const TextStyle(
                            fontSize: 10, color: AppTheme.dim),
                      ),
                    ),
                  ),
                  bottomTitles: AxisTitles(
                    sideTitles: SideTitles(
                      showTitles: true,
                      getTitlesWidget: (v, _) {
                        final i = v.toInt();
                        if (i < 0 || i >= _days!.length) {
                          return const SizedBox.shrink();
                        }
                        final d = _days![i].date;
                        // Show abbreviated date
                        final short = d.length >= 5 ? d.substring(5) : d;
                        return Text(short,
                            style: const TextStyle(
                                fontSize: 9, color: AppTheme.dim));
                      },
                    ),
                  ),
                  topTitles: const AxisTitles(
                      sideTitles: SideTitles(showTitles: false)),
                  rightTitles: const AxisTitles(
                      sideTitles: SideTitles(showTitles: false)),
                ),
                borderData: FlBorderData(show: false),
                barGroups: List.generate(_days!.length, (i) {
                  return BarChartGroupData(
                    x: i,
                    barRods: [
                      BarChartRodData(
                        toY: _days![i].hours,
                        width: _days!.length > 14 ? 6 : 12,
                        color: AppTheme.accent,
                        borderRadius: const BorderRadius.vertical(
                            top: Radius.circular(3)),
                      ),
                    ],
                  );
                }),
                barTouchData: BarTouchData(
                  touchTooltipData: BarTouchTooltipData(
                    getTooltipItem: (group, gIdx, rod, rIdx) {
                      final d = _days![group.x];
                      return BarTooltipItem(
                        '${d.date}\n${d.hours.toStringAsFixed(1)} h',
                        const TextStyle(
                            color: AppTheme.accent,
                            fontSize: 12,
                            fontWeight: FontWeight.w600),
                      );
                    },
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _StatChip extends StatelessWidget {
  final String label;
  final String value;

  const _StatChip({required this.label, required this.value});

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Text(label,
            style: const TextStyle(fontSize: 10, color: AppTheme.dim)),
        const SizedBox(height: 2),
        Text(value,
            style: const TextStyle(
                fontSize: 15,
                fontWeight: FontWeight.w700,
                fontFamily: 'monospace')),
      ],
    );
  }
}
