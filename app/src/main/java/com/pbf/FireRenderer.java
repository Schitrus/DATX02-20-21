package com.pbf;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import java.util.Queue;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import androidx.core.content.ContextCompat;

public class FireRenderer implements GLSurfaceView.Renderer {

    private final Queue<Runnable> taskQueue;
    private final Context context;

    private ImageView loading;

    FireRenderer(Queue<Runnable> taskQueue, Context context, ImageView loading) {
        this.taskQueue = taskQueue;
        this.context = context;
        this.loading = loading;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String extensions = gl.glGetString(gl.GL_EXTENSIONS);
        boolean has_float_buffer = (extensions.contains("EXT_color_buffer_float"));
        if (!has_float_buffer) {
            Log.e("FIRE", "EXT_color_buffer_float not supported, terminating program. . .");
            System.exit(-1);
        }
        if(init() == 0){
            Log.e("FIRE", "Failed to initialize the fire. . .");
            System.exit(-1);
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        executeTasks();
        ContextCompat.getMainExecutor(context).execute(new Runnable() {
            @Override
            public void run() {
                if(changedSettings())
                    loading.setVisibility(View.VISIBLE);
                else
                    loading.setVisibility(View.INVISIBLE);
            }
        });
        update();

        FPSCounter.logFrame();
    }

    private void executeTasks() {
        Runnable task;
        do {
            task = taskQueue.poll();
            if(task != null)
                task.run();
        } while(task != null);
    }

    public native boolean changedSettings();
    public native void update();
    public native void resize(int width, int height);
    private native int init();

}

class FPSCounter {
    private static long startTime = System.nanoTime();
    private static int frames = 0;

    static public void logFrame() {
        frames++;
        if(System.nanoTime() - startTime >= 1000000000) {
            Log.d("FPSCounter", "fps: " + frames);
            frames = 0;
            startTime = System.nanoTime();
        }
    }
}
