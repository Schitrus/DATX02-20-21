package com.pbf;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Point;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;


import androidx.constraintlayout.widget.ConstraintLayout;

import com.example.datx02_20_21.R;

public class FireActivity extends Activity {


    // Used to load the 'fire-lib' library on application startup.
    static {
        System.loadLibrary("fire-lib");
    }

    private ConstraintLayout mainLayout;
    private FireView fire;
    private Button settingsButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mainLayout = findViewById(R.id.mainLayout);

        settingsButton = findViewById(R.id.settingsButton);
        settingsButton.setOnClickListener(new SettingsButtonClickListener());

        fire = new FireView(getApplication());

        Point dimension = new Point();
        getWindowManager().getDefaultDisplay().getSize(dimension);

        init(getResources().getAssets(), dimension.x, dimension.y);
        mainLayout.addView(fire, 0);

        setContentView(mainLayout);

        //setContentView(fire);
    }

    private class SettingsButtonClickListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            Intent intent  = new Intent(FireActivity.this, SettingsActivity.class);
            FireActivity.this.startActivity(intent);
        }
    }

    public native void init(AssetManager mgr, int width, int height);

}
