package com.pbf;

import android.content.res.AssetManager;
import android.graphics.Point;
import android.graphics.drawable.AnimationDrawable;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.transition.AutoTransition;
import android.transition.Fade;
import android.transition.Slide;
import android.transition.TransitionManager;
import android.transition.TransitionSet;
import android.view.Gravity;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.ToggleButton;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.FragmentActivity;

import com.example.datx02_20_21.R;

public class FireActivity extends FragmentActivity {


    // Used to load the 'fire-lib' library on application startup.
    static {
        System.loadLibrary("fire-lib");
    }

    private SettingsFragment settingsFragment;
    private ConstraintLayout mainLayout;
    private ConstraintLayout constraintLayout;
    private ConstraintLayout scrollView;
    private ToggleButton settingsButton;
    private FireView view;

    private AnimationDrawable animation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Point dimension = new Point();
        getWindowManager().getDefaultDisplay().getSize(dimension);
        init(getResources().getAssets(), dimension.x, dimension.y);

        mainLayout = findViewById(R.id.mainLayout);

        constraintLayout = findViewById(R.id.constraintLayout);
        scrollView = findViewById(R.id.scrollViewContainer);

        findViewById(R.id.scrollViewContainer).setVisibility(View.INVISIBLE);

        settingsFragment = new SettingsFragment();
        settingsButton = findViewById(R.id.toggleSettingsButton);
        settingsButton.setOnCheckedChangeListener(new SettingsButtonToggleListener());

        ImageView loading = (ImageView)findViewById(R.id.loadingImage);
        animation = (AnimationDrawable)loading.getDrawable();
        animation.start();

        view = new FireView(getApplication(), (SensorManager) getSystemService(SENSOR_SERVICE));
        // Prepend so that settings UI is placed on top of the FireView
        mainLayout.addView(view, 0);

        setContentView(mainLayout);

    }



    private class SettingsButtonToggleListener implements CompoundButton.OnCheckedChangeListener {

        private final int DURATION_IN_MILLISECONDS = 250;
        private final int MARGIN = 8;

        SettingsButtonToggleListener(){
            super();
            initFragment();
        }

        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean checked) {
            if(checked){
                //moveButtonDown();
                showFragment();
                rotateButtonCounterCW();
            }
            else{
                //moveButtonUp();
                hideFragment();
                rotateButtonCW();
            }
        }

        private void initFragment(){

            Slide slideTransition = new Slide(Gravity.TOP);
            slideTransition.setDuration(DURATION_IN_MILLISECONDS);
            Fade fadeTransition = new Fade();
            fadeTransition.setDuration(DURATION_IN_MILLISECONDS);

            TransitionSet transition = new TransitionSet();
            transition.addTransition(slideTransition).addTransition(fadeTransition);

            settingsFragment.setEnterTransition(transition);
            settingsFragment.setExitTransition(transition);

            findViewById(R.id.scrollViewContainer).setVisibility(View.INVISIBLE);

            getSupportFragmentManager().beginTransaction().replace(R.id.settingsContainer, settingsFragment).commitNow();

            getSupportFragmentManager().beginTransaction().hide(settingsFragment).commitNow();

        }

        private void showFragment(){

            findViewById(R.id.scrollViewContainer).setVisibility(View.VISIBLE);

            getSupportFragmentManager().beginTransaction().show(settingsFragment).commitNow();
        }

        private void hideFragment() {
            //for (Fragment fragment : getSupportFragmentManager().getFragments()) {
              //  getSupportFragmentManager().beginTransaction().remove(fragment).commit();
            //}

            getSupportFragmentManager().beginTransaction().hide(settingsFragment).commitNow();

            Thread t = new Thread(){
                @Override
                public void run() {
                    try {
                        Thread.sleep(DURATION_IN_MILLISECONDS);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                findViewById(R.id.scrollViewContainer).setVisibility(View.INVISIBLE);
                            }
                        });
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            };

            t.start();

        }

        // Rotate Button 180 degress counter-clockwise
        private void rotateButtonCounterCW(){
            settingsButton.animate().setDuration(DURATION_IN_MILLISECONDS).rotation(180);
        }

        // Rotate Button 180 degress clockwise
        private void rotateButtonCW(){
            settingsButton.animate().setDuration(DURATION_IN_MILLISECONDS).rotation(0);
        }

        private void moveButtonDown(){
            constrainButtonTopToView(R.id.scrollViewContainer, ConstraintSet.BOTTOM);
        }

        private void moveButtonUp(){
           constrainButtonTopToView(R.id.constraintLayout, ConstraintSet.TOP);
        }

        private void constrainButtonTopToView(int viewID, int side){
            beginTransition();

            ConstraintSet constraintSet = new ConstraintSet();
            constraintSet.clone(constraintLayout);
            constraintSet.connect(
                    R.id.toggleSettingsButton,
                    ConstraintSet.TOP,
                    viewID,
                    side,
                    MARGIN
            );
            constraintSet.applyTo(constraintLayout);
        }

        private void beginTransition(){
            AutoTransition autoTransition = new AutoTransition();
            autoTransition.setDuration(DURATION_IN_MILLISECONDS);
            TransitionManager.beginDelayedTransition(constraintLayout, autoTransition);
        }
    }

    public native void init(AssetManager mgr, int width, int height);

}
