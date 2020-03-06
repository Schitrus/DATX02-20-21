package com.pbf;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FireRenderer implements GLSurfaceView.Renderer {

    // Used to load the 'fire-lib' library on application startup.
    static {
        System.loadLibrary("fire-lib");
    }

    private Context context;

    public FireRenderer(Context context){
        this.context = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        AssetManager mgr = context.getResources().getAssets();
        init(mgr);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        resize(width, height);
        //resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        update();
        //step();
        FPSCounter.logFrame();
    }

    public native void update();
    public native void resize(int width, int height);
    private native void init(AssetManager mgr);
    //private native void step();
    //private native void resize(int width, int height);

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
