package com.bearwaves.growl;

public class HapticsEffect {
    public final double duration;
    public final float intensity;
    public final float sharpness;
    public final float delay;

    public HapticsEffect(double duration, float intensity, float sharpness, float delay) {
        this.duration = duration;
        this.intensity = intensity;
        this.sharpness = sharpness;
        this.delay = delay;
    }
}
