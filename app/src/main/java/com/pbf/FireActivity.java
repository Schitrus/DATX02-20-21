package com.pbf;

import android.content.res.AssetManager;
import android.graphics.Point;
import android.os.Bundle;
import android.transition.AutoTransition;
import android.transition.Fade;
import android.transition.Slide;
import android.transition.TransitionManager;
import android.transition.TransitionSet;
import android.view.Gravity;
import android.widget.CompoundButton;
import android.widget.ToggleButton;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;
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
    private ToggleButton settingsButton;
    private FireView fire;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        settingsFragment = new SettingsFragment();

        mainLayout = findViewById(R.id.mainLayout);

        constraintLayout = findViewById(R.id.constraintLayout);

        settingsButton = findViewById(R.id.toggleSettingsButton);
        settingsButton.setOnCheckedChangeListener(new SettingsButtonToggleListener());

        fire = new FireView(getApplication());

        Point dimension = new Point();
        getWindowManager().getDefaultDisplay().getSize(dimension);

        init(getResources().getAssets(), dimension.x, dimension.y);
        // Prepend so that settings UI is placed on top of the FireView
        mainLayout.addView(fire, 0);

        setContentView(mainLayout);

        //setContentView(fire);
    }

    private class SettingsButtonToggleListener implements CompoundButton.OnCheckedChangeListener {

        private final int DURATION_IN_MILLISECONDS = 250;
        private final int MARGIN = 8;

        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean checked) {
            if(checked){
                //moveButtonDown();
                addFragment();
                rotateButtonCounterCW();
            }
            else{
                //moveButtonUp();
                removeFragment();
                rotateButtonCW();
            }
        }

        private void addFragment(){
            Slide slideTransition = new Slide(Gravity.TOP);
            slideTransition.setDuration(DURATION_IN_MILLISECONDS);
            Fade fadeTransition = new Fade();
            fadeTransition.setDuration(DURATION_IN_MILLISECONDS);

            TransitionSet transitionSet = new TransitionSet();
            transitionSet.addTransition(slideTransition).addTransition(fadeTransition);
            
            settingsFragment.setEnterTransition(transitionSet);
            settingsFragment.setExitTransition(transitionSet);
            getSupportFragmentManager().beginTransaction()
                    .replace(R.id.settingsContainer, settingsFragment)
                    .commit();
        }

        private void removeFragment(){
            //for (Fragment fragment : getSupportFragmentManager().getFragments()) {
              //  getSupportFragmentManager().beginTransaction().remove(fragment).commit();
            //}

            getSupportFragmentManager().beginTransaction().remove(settingsFragment).commit();
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
