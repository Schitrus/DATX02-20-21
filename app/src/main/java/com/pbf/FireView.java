package com.pbf;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import androidx.appcompat.widget.AppCompatSeekBar;

import com.example.datx02_20_21.R;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class FireView extends GLSurfaceView {

    public FireRenderer renderer;
    public FireListener listener;

    public FireView(Context context, ImageView loading){
        super(context);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        // A queue used to send tasks from the listener to the renderer, to make sure that all calls to the native library happens on the same thread and with a gl-context
        Queue<Runnable> inputTaskQueue = new ConcurrentLinkedQueue<>();

        renderer = new FireRenderer(inputTaskQueue, context, loading);
        setRenderer(renderer);

        listener = new FireListener(inputTaskQueue, context);
        setOnTouchListener(listener);
        setOnClickListener(listener);
    }

}
