package com.example.datx02_20_21;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class FireView extends GLSurfaceView {

    public FireRenderer renderer;
    public FireListener listener;

    public FireView(Context context){
        super(context);
        init(context);
    }

    public void init(Context context){

        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        renderer = new FireRenderer();
        setRenderer(renderer);

        listener = new FireListener(context);
        setOnTouchListener(listener);
    }

}
