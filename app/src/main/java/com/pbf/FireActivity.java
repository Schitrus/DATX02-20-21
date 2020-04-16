package com.pbf;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Point;
import android.os.Bundle;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.appcompat.widget.AppCompatSeekBar;

import com.example.datx02_20_21.R;

public class FireActivity extends Activity {


    // Used to load the 'fire-lib' library on application startup.
    static {
        System.loadLibrary("fire-lib");
    }

    private FireView fire;

    private TextView textView;
    private ProgressBar progressBar;
    private SeekBar seekBar;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Point dimension = new Point();
        getWindowManager().getDefaultDisplay().getSize(dimension);

        init(getResources().getAssets(), dimension.x, dimension.y);

        fire = new FireView(getApplication());

        setContentView(fire);

        textView = (TextView) findViewById(R.id.textView);
        progressBar = (ProgressBar) findViewById(R.id.progressBar);
        seekBar = (SeekBar) findViewById(R.id.seekBar);

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean b) {
                progressBar.setProgress(progress); 

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

    }

    public native void init(AssetManager mgr, int width, int height);

}
