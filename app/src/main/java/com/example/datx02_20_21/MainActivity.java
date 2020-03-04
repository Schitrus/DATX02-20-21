package com.example.datx02_20_21;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {

    // Used to load the 'fire-lib' library on application startup.
    static {
        System.loadLibrary("fire-lib");
    }

    private FireView fire;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        fire = new FireView(getApplication());
        setContentView(fire);

    }


}
