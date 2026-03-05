/// HTTP client for communicating with the ESP32 SmartGen HSC941 Web API.
library;

import 'dart:async';
import 'dart:convert';
import 'package:http/http.dart' as http;
import '../models/genset_status.dart';
import '../models/config_models.dart';

class GensetApiClient {
  String _baseUrl;
  final http.Client _client;
  static const Duration _timeout = Duration(seconds: 5);

  GensetApiClient({required String host, int port = 80, http.Client? client})
      : _baseUrl = 'http://$host:$port',
        _client = client ?? http.Client();

  String get baseUrl => _baseUrl;

  void updateHost(String host, {int port = 80}) {
    _baseUrl = 'http://$host:$port';
  }

  void dispose() {
    _client.close();
  }

  // ── Status ──
  Future<GensetStatus> getStatus() async {
    final resp = await _client
        .get(Uri.parse('$_baseUrl/api/status'))
        .timeout(_timeout);
    if (resp.statusCode != 200) throw ApiException('Status: ${resp.statusCode}');
    return GensetStatus.fromJson(json.decode(resp.body) as Map<String, dynamic>);
  }

  // ── Commands ──
  Future<bool> sendCommand(int coilIndex) async {
    final resp = await _client
        .post(
          Uri.parse('$_baseUrl/api/command'),
          headers: {'Content-Type': 'application/json'},
          body: json.encode({'coil': coilIndex}),
        )
        .timeout(_timeout);
    if (resp.statusCode != 200) return false;
    final data = json.decode(resp.body) as Map<String, dynamic>;
    return data['ok'] == true;
  }

  Future<bool> startEngine() => sendCommand(0);
  Future<bool> stopEngine() => sendCommand(1);
  Future<bool> setAutoMode() => sendCommand(3);
  Future<bool> setManualMode() => sendCommand(4);
  Future<bool> genSwitchOff() => sendCommand(5);
  Future<bool> genSwitchOn() => sendCommand(6);
  Future<bool> faultReset() => sendCommand(7);

  // ── Relay control ──
  Future<bool> toggleRelay(int index) async {
    final resp = await _client
        .post(
          Uri.parse('$_baseUrl/api/relay'),
          headers: {'Content-Type': 'application/json'},
          body: json.encode({'index': index}),
        )
        .timeout(_timeout);
    if (resp.statusCode != 200) return false;
    final data = json.decode(resp.body) as Map<String, dynamic>;
    return data['ok'] == true;
  }

  // ── Exercise schedule ──
  Future<ExerciseConfig> getExercise() async {
    final resp = await _client
        .get(Uri.parse('$_baseUrl/api/exercise'))
        .timeout(_timeout);
    if (resp.statusCode != 200) {
      throw ApiException('Exercise GET: ${resp.statusCode}');
    }
    return ExerciseConfig.fromJson(
        json.decode(resp.body) as Map<String, dynamic>);
  }

  Future<bool> saveExercise(ExerciseConfig config) async {
    final resp = await _client
        .post(
          Uri.parse('$_baseUrl/api/exercise'),
          headers: {'Content-Type': 'application/json'},
          body: json.encode(config.toJson()),
        )
        .timeout(_timeout);
    if (resp.statusCode != 200) return false;
    final data = json.decode(resp.body) as Map<String, dynamic>;
    return data['ok'] == true;
  }

  Future<bool> runExercise() async {
    final resp = await _client
        .post(
          Uri.parse('$_baseUrl/api/exercise'),
          headers: {'Content-Type': 'application/json'},
          body: json.encode({'action': 'run'}),
        )
        .timeout(_timeout);
    if (resp.statusCode != 200) return false;
    final data = json.decode(resp.body) as Map<String, dynamic>;
    return data['ok'] == true;
  }

  Future<bool> stopExercise() async {
    final resp = await _client
        .post(
          Uri.parse('$_baseUrl/api/exercise'),
          headers: {'Content-Type': 'application/json'},
          body: json.encode({'action': 'stop'}),
        )
        .timeout(_timeout);
    if (resp.statusCode != 200) return false;
    final data = json.decode(resp.body) as Map<String, dynamic>;
    return data['ok'] == true;
  }

  // ── Fuel ──
  Future<FuelStatus> getFuel() async {
    final resp = await _client
        .get(Uri.parse('$_baseUrl/api/fuel'))
        .timeout(_timeout);
    if (resp.statusCode != 200) throw ApiException('Fuel: ${resp.statusCode}');
    return FuelStatus.fromJson(json.decode(resp.body) as Map<String, dynamic>);
  }

  // ── Maintenance ──
  Future<Map<String, dynamic>> getMaintenance() async {
    final resp = await _client
        .get(Uri.parse('$_baseUrl/api/maintenance'))
        .timeout(_timeout);
    if (resp.statusCode != 200) {
      throw ApiException('Maint: ${resp.statusCode}');
    }
    return json.decode(resp.body) as Map<String, dynamic>;
  }

  // ── Event log ──
  Future<List<EventEntry>> getEventLog() async {
    final resp = await _client
        .get(Uri.parse('$_baseUrl/api/eventlog'))
        .timeout(_timeout);
    if (resp.statusCode != 200) {
      throw ApiException('EventLog: ${resp.statusCode}');
    }
    final data = json.decode(resp.body) as Map<String, dynamic>;
    final events = data['events'] as List? ?? [];
    return events
        .map((e) => EventEntry.fromJson(e as Map<String, dynamic>))
        .toList();
  }

  // ── Battery history ──
  Future<BatteryHistory> getBatteryHistory() async {
    final resp = await _client
        .get(Uri.parse('$_baseUrl/api/battery_history'))
        .timeout(_timeout);
    if (resp.statusCode != 200) {
      throw ApiException('BattHist: ${resp.statusCode}');
    }
    return BatteryHistory.fromJson(
        json.decode(resp.body) as Map<String, dynamic>);
  }

  // ── Runtime history ──
  Future<List<RuntimeDay>> getRuntimeHistory() async {
    final resp = await _client
        .get(Uri.parse('$_baseUrl/api/runtime_history'))
        .timeout(_timeout);
    if (resp.statusCode != 200) {
      throw ApiException('RuntimeHist: ${resp.statusCode}');
    }
    final data = json.decode(resp.body) as Map<String, dynamic>;
    final days = data['days'] as List? ?? [];
    return days
        .map((e) => RuntimeDay.fromJson(e as Map<String, dynamic>))
        .toList();
  }

  // ── Controller config parameters ──
  Future<ConfigData> getConfig() async {
    final resp = await _client
        .get(Uri.parse('$_baseUrl/api/config'))
        .timeout(const Duration(seconds: 10)); // longer — reads Modbus
    if (resp.statusCode != 200) {
      throw ApiException('Config: ${resp.statusCode}');
    }
    final data = json.decode(resp.body) as Map<String, dynamic>;
    if (data['ok'] != true) {
      throw ApiException(data['error'] as String? ?? 'Config read failed');
    }
    return ConfigData.fromJson(data);
  }

  // ── Buzzer ──
  Future<bool> silenceBuzzer() async {
    final resp = await _client
        .post(
          Uri.parse('$_baseUrl/api/buzzer'),
          headers: {'Content-Type': 'application/json'},
          body: json.encode({'action': 'silence'}),
        )
        .timeout(_timeout);
    if (resp.statusCode != 200) return false;
    final data = json.decode(resp.body) as Map<String, dynamic>;
    return data['ok'] == true;
  }
}

class ApiException implements Exception {
  final String message;
  ApiException(this.message);

  @override
  String toString() => 'ApiException: $message';
}
