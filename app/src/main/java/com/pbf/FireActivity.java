package com.pbf;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Point;
import android.hardware.SensorManager;
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

        init(getResources().getAssets(), dimension.x, dimension.y);

        fire = new FireView(getApplication(), (SensorManager) getSystemService(SENSOR_SERVICE));

        setContentView(fire);

    }

    public native void init(AssetManager mgr, int width, int height);

}
