package com.example.datx02_20_21;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class FireView extends GLSurfaceView {

    public FireRenderer renderer;

    public FireView(Context context){
        super(context);
        init();
    }

    public void init(){

        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        renderer = new FireRenderer();
        setRenderer(renderer);
    }

}
