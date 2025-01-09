package com.bearwaves.growl;

import android.os.Build;
import android.os.Bundle;
import android.view.WindowInsets;
import androidx.activity.EdgeToEdge;
import com.google.androidgamesdk.GameActivity;

public class GrowlActivity extends GameActivity {

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
}
