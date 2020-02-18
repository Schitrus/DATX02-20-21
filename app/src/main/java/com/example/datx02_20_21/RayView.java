package com.example.datx02_20_21;

import android.content.Context;
import android.opengl.GLSurfaceView;


public class RayView extends GLSurfaceView {

    private RayRenderer renderer;

    public RayView(Context context) {
        super(context);

        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        renderer = new RayRenderer();
        setRenderer(renderer);
    }

}
