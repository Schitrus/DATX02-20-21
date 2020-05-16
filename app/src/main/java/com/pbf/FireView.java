package com.pbf;

import android.content.Context;
import android.opengl.GLSurfaceView;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class FireView extends GLSurfaceView {

    public FireView(Context context){
        super(context);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        // A queue used to send tasks from the listener to the renderer, to make sure that all calls to the native library happens on the same thread and with a gl-context
        Queue<Runnable> inputTaskQueue = new ConcurrentLinkedQueue<>();

        setRenderer(new FireRenderer(inputTaskQueue, context));

        FireListener listener = new FireListener(inputTaskQueue, context);
        setOnTouchListener(listener);
        setOnClickListener(listener);
    }
}
