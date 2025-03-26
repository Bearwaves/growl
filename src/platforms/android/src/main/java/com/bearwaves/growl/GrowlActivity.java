package com.bearwaves.growl;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.view.WindowInsets;
import androidx.activity.EdgeToEdge;
import com.google.androidgamesdk.GameActivity;

public class GrowlActivity extends GameActivity {

    private static final String GROWL_PREFERENCES_KEY_LOCAL = "growl.preferences-local";
    private static final String GROWL_PREFERENCES_KEY_SHARED = "growl.preferences-shared";

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
}
