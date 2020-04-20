package com.pbf;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import androidx.appcompat.widget.AppCompatSeekBar;

import com.example.datx02_20_21.R;

public class FireView extends GLSurfaceView {

    public FireRenderer renderer;
    public FireListener listener;


    public FireView(Context context) {
        super(context);
        init(context);
    }

    public void init(Context context) {

        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        renderer = new FireRenderer(context);
        setRenderer(renderer);

        listener = new FireListener(context);
        setOnTouchListener(listener);
    }

}
