/// App theme — dark industrial SCADA style matching the web dashboard.
library;

import 'package:flutter/material.dart';

class AppTheme {
  // Colors matching the web dashboard CSS variables
  static const Color background = Color(0xFF0C0E14);
  static const Color surface = Color(0xFF14171F);
  static const Color card = Color(0xFF181C26);
  static const Color border = Color(0xFF252A36);
  static const Color text = Color(0xFFE1E4EA);
  static const Color dim = Color(0xFF6B7280);
  static const Color faint = Color(0xFF3A3F4B);
  static const Color accent = Color(0xFF3B82F6);
  static const Color green = Color(0xFF22C55E);
  static const Color red = Color(0xFFEF4444);
  static const Color orange = Color(0xFFF59E0B);
  static const Color cyan = Color(0xFF06B6D4);
  static const Color blue = Color(0xFF3B82F6);

  static ThemeData get darkTheme {
    return ThemeData(
      useMaterial3: true,
      brightness: Brightness.dark,
      scaffoldBackgroundColor: background,
      colorScheme: const ColorScheme.dark(
        surface: surface,
        primary: accent,
        secondary: cyan,
        error: red,
        onPrimary: Colors.white,
        onSurface: text,
        outline: border,
      ),
      cardTheme: CardThemeData(
        color: card,
        elevation: 0,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(12),
          side: const BorderSide(color: border, width: 1),
        ),
      ),
      appBarTheme: const AppBarTheme(
        backgroundColor: surface,
        foregroundColor: text,
        elevation: 0,
        scrolledUnderElevation: 1,
      ),
      bottomNavigationBarTheme: const BottomNavigationBarThemeData(
        backgroundColor: surface,
        selectedItemColor: accent,
        unselectedItemColor: dim,
      ),
      navigationBarTheme: NavigationBarThemeData(
        backgroundColor: surface,
        indicatorColor: accent.withAlpha(40),
        labelTextStyle: WidgetStateProperty.resolveWith((states) {
          if (states.contains(WidgetState.selected)) {
            return const TextStyle(
                fontSize: 11, fontWeight: FontWeight.w600, color: accent);
          }
          return const TextStyle(fontSize: 11, color: dim);
        }),
      ),
      elevatedButtonTheme: ElevatedButtonThemeData(
        style: ElevatedButton.styleFrom(
          backgroundColor: accent,
          foregroundColor: Colors.white,
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(8)),
          padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 12),
        ),
      ),
      textButtonTheme: TextButtonThemeData(
        style: TextButton.styleFrom(foregroundColor: accent),
      ),
      outlinedButtonTheme: OutlinedButtonThemeData(
        style: OutlinedButton.styleFrom(
          foregroundColor: text,
          side: const BorderSide(color: border),
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(8)),
        ),
      ),
      dividerTheme: const DividerThemeData(color: border, thickness: 1),
      chipTheme: ChipThemeData(
        backgroundColor: surface,
        side: const BorderSide(color: border),
        labelStyle: const TextStyle(fontSize: 12, color: text),
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
      ),
      textTheme: const TextTheme(
        headlineLarge:
            TextStyle(fontSize: 24, fontWeight: FontWeight.w700, color: text),
        headlineMedium:
            TextStyle(fontSize: 20, fontWeight: FontWeight.w700, color: text),
        titleLarge:
            TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: text),
        titleMedium:
            TextStyle(fontSize: 14, fontWeight: FontWeight.w600, color: text),
        bodyLarge: TextStyle(fontSize: 14, color: text),
        bodyMedium: TextStyle(fontSize: 13, color: text),
        bodySmall: TextStyle(fontSize: 11, color: dim),
        labelSmall: TextStyle(
            fontSize: 10,
            fontWeight: FontWeight.w600,
            letterSpacing: 0.8,
            color: dim),
      ),
      snackBarTheme: SnackBarThemeData(
        backgroundColor: card,
        contentTextStyle: const TextStyle(color: text),
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(8)),
        behavior: SnackBarBehavior.floating,
      ),
      dialogTheme: DialogThemeData(
        backgroundColor: card,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(14)),
      ),
    );
  }
}
