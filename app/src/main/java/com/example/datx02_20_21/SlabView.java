package com.example.datx02_20_21;

import android.content.Context;
import android.opengl.GLSurfaceView;


public class SlabView extends GLSurfaceView {

    private SlabRenderer renderer;

    public SlabView(Context context) {
        super(context);

        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        renderer = new SlabRenderer(context);
        setRenderer(renderer);
    }

}
