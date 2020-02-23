package com.example.datx02_20_21;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {

    private SlabView view;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        view = new SlabView(getApplication());
        setContentView(view);

    }


}
