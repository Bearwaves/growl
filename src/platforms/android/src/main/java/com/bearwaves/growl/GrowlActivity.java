package com.bearwaves.growl;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.VibratorManager;
import android.view.WindowInsets;

import androidx.activity.EdgeToEdge;

import com.google.androidgamesdk.GameActivity;

public class GrowlActivity extends GameActivity {

    private static final String GROWL_PREFERENCES_KEY_LOCAL = "growl.preferences-local";
    private static final String GROWL_PREFERENCES_KEY_SHARED = "growl.preferences-shared";
    private static final int TAP_VIBRATION_MS = 20;
    private static final float VIBRATION_INTENSITY_SCALAR = 0.5f;
    private static final float VIBRATION_SOFTNESS_SCALAR = 0.25f;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        EdgeToEdge.enable(this);
        super.onCreate(savedInstanceState);
    }

    public int[] getInsets() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            WindowInsets insets = getWindow().getDecorView().getRootWindowInsets();
            if (insets != null) {
                return new int[]{
                        insets.getSystemWindowInsetTop(),
                        insets.getSystemWindowInsetBottom(),
                        insets.getSystemWindowInsetLeft(),
                        insets.getSystemWindowInsetRight(),
                };
            }
        }
        return new int[]{0, 0, 0, 0};
    }

    public boolean getDarkMode() {
        return (getResources().getConfiguration().uiMode & Configuration.UI_MODE_NIGHT_MASK) == Configuration.UI_MODE_NIGHT_YES;
    }

    public String getPreferencesJSON(boolean shared) {
        SharedPreferences preferences = getPreferences(Context.MODE_PRIVATE);
        return preferences.getString(shared ? GROWL_PREFERENCES_KEY_SHARED : GROWL_PREFERENCES_KEY_LOCAL, "{}");
    }

    public void setPreferencesJSON(boolean shared, String preferencesJSON) {
        SharedPreferences preferences = getPreferences(Context.MODE_PRIVATE);
        preferences.edit().putString(
                shared ? GROWL_PREFERENCES_KEY_SHARED : GROWL_PREFERENCES_KEY_LOCAL, preferencesJSON
        ).apply();
    }

    public boolean supportsHaptics() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return false;
        }
        Vibrator vibrator = getVibrator();
        return vibrator != null && vibrator.hasVibrator() && vibrator.hasAmplitudeControl();
    }

    public void playVibrationPattern(HapticsEffect... pattern) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return;
        }
        // We can't properly use sharpness until Android 16.
        int[] intensities = new int[pattern.length * 2];
        long[] timings = new long[pattern.length * 2];
        for (int i = 0; i < pattern.length; i++) {
            HapticsEffect effect = pattern[i];
            intensities[i * 2] = 0;
            // We don't have sharpness so just reduce the intensity by up to half.
            intensities[i * 2 + 1] = (int) (effect.intensity * VIBRATION_INTENSITY_SCALAR *
                    (VIBRATION_SOFTNESS_SCALAR + effect.sharpness * (1 - VIBRATION_SOFTNESS_SCALAR))
                    * 255);
            timings[i * 2] = (int) (effect.delay * 1000);
            int durationMs = (int) (effect.duration * 1000);
            if (durationMs == 0) {
                durationMs = TAP_VIBRATION_MS;
            }
            timings[(i * 2) + 1] = durationMs;
        }
        VibrationEffect waveform = VibrationEffect.createWaveform(timings, intensities, -1);
        getVibrator().vibrate(waveform);
    }

    private Vibrator getVibrator() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            return ((VibratorManager) getSystemService(VIBRATOR_MANAGER_SERVICE)).getDefaultVibrator();
        }
        return (Vibrator) getSystemService(VIBRATOR_SERVICE);
    }
}
