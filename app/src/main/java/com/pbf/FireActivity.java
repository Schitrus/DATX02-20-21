package com.pbf;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Point;
import android.os.Bundle;

import com.example.datx02_20_21.R;

public class FireActivity extends Activity {

    // Used to load the 'fire-lib' library on application startup.
    static {
        System.loadLibrary("fire-lib");
    }

    private FireView fire;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Point dimension = new Point();
        getWindowManager().getDefaultDisplay().getSize(dimension);

        init(dimension.x, dimension.y);

        AssetManager mgr = getResources().getAssets();
        initFileLoader(mgr);

        fire = new FireView(getApplication());

        setContentView(fire);

    }

    public native void init(int width, int height);

    private native void initFileLoader(AssetManager manager);

}
