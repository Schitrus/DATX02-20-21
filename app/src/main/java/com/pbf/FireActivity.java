package com.pbf;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Point;
import android.os.Bundle;
import android.text.Layout;
import android.widget.FrameLayout;
import android.widget.SeekBar;


import com.example.datx02_20_21.R;

public class FireActivity extends Activity {


    // Used to load the 'fire-lib' library on application startup.
    static {
        System.loadLibrary("fire-lib");
    }

    private FireView fire;
    private SeekBar seekBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        FrameLayout fl = new FrameLayout(getApplicationContext());
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT
        );

        //seekBar = (SeekBar) findViewById(R.id.seekBar);
        fire = new FireView(getApplication());
        seekBar = new SeekBar(this);

        Point dimension = new Point();
        getWindowManager().getDefaultDisplay().getSize(dimension);

        init(getResources().getAssets(), dimension.x, dimension.y);
        fl.addView(fire);
        fl.addView(seekBar);

        setContentView(fl);

        //setContentView(fire);


    }

    public native void init(AssetManager mgr, int width, int height);

}
