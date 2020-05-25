package com.pbf;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.NumberPicker;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.example.datx02_20_21.R;
import com.flask.colorpicker.ColorPickerView;
import com.flask.colorpicker.OnColorChangedListener;
import com.flask.colorpicker.OnColorSelectedListener;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.text.DecimalFormat;

public class SettingsFragment extends Fragment {

    private static final int VORTICITY_MIN = 0;
    private static final int VORTICITY_MAX = 1000;
    private static final float VORTICITY_STEP = 0.05f;
    private static final int ITERATIONS_MIN = 1;
    private static final int ITERATIONS_MAX = 100;
    private static final float ITERATIONS_STEP = 1.0f;
    private static final int BUOYANCY_MIN = 0;
    private static final int BUOYANCY_MAX = 100;
    private static final float BUOYANCY_STEP = 0.1f;
    private static final int VISCOSITY_MIN = 1;
    private static final int VISCOSITY_MAX = 1000;
    private static final float VISCOSITY_STEP = 0.01f;
    private static final int WIND_MIN = 0;
    private static final int WIND_MAX = 1000;
    private static final float WIND_STEP = 0.25f;

    private enum ResolutionItems{
            LOW, MEDIUM, HIGH;

        public static String[] stringValues(){
                ResolutionItems[] resolutionItems = ResolutionItems.values();
                String[] values = new String[resolutionItems.length];
                for(int i = 0; i < resolutionItems.length; i++){
                    values[i] = resolutionItems[i].name();
                }
                return values;
        }
    };

    private SeekBar vorticityBar;
    private TextView vorticityValueText;

    private SeekBar iterationsBar;
    private TextView iterationsValueText;

    private SeekBar buoyancyBar;
    private TextView buoyancyValueText;

    private SeekBar viscosityBar;
    private TextView viscosityValueText;

    private SeekBar windBar;
    private TextView windValueText;

    private Spinner resolutionSpinner;

    private ColorPickerView backgroundColor, filterColor;

    private NumberPicker colorSpaceX, colorSpaceY, colorSpaceZ;

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.settings_fragment, container, false);
        initUIElements(view);
        return view;
    }

    private void initUIElements(View v){
        initVorticityBar(v);
        initIterationsBar(v);
        initBuoyancyBar(v);
        initViscosityBar(v);
        initWindBar(v);
        initResolutionSpinner(v);
        initColorPicker(v);
        initColorSpacePicker(v);
    }

    private void initVorticityBar(View v){
        vorticityBar = v.findViewById(R.id.vorticityBar);
        vorticityValueText = v.findViewById(R.id.vorticityValueText);

        vorticityBar.setOnSeekBarChangeListener(
                new SliderBarListener(vorticityBar, vorticityValueText, VORTICITY_MIN, VORTICITY_MAX, VORTICITY_STEP)
        );
    }

    private void initIterationsBar(View v){
        iterationsBar = v.findViewById(R.id.projectionIterationsBar);
        iterationsValueText = v.findViewById(R.id.projectionIterationsValueText);

       iterationsBar.setOnSeekBarChangeListener(
               new SliderBarListener(iterationsBar, iterationsValueText, ITERATIONS_MIN, ITERATIONS_MAX, ITERATIONS_STEP)
       );
    }

    private void initBuoyancyBar(View v){
        buoyancyBar = v.findViewById(R.id.buoyancyBar);
        buoyancyValueText = v.findViewById(R.id.buoyancyValueText);

        buoyancyBar.setOnSeekBarChangeListener(
                new SliderBarListener(buoyancyBar, buoyancyValueText, BUOYANCY_MIN, BUOYANCY_MAX, BUOYANCY_STEP)
        );
    }

    private void initViscosityBar(View v){
        viscosityBar = v.findViewById(R.id.smokeKinematicViscosityBar);
        viscosityValueText = v.findViewById(R.id.smokeKinematicViscosityValueText);

        viscosityBar.setOnSeekBarChangeListener(
                new SliderBarListener(viscosityBar, viscosityValueText, VISCOSITY_MIN, VISCOSITY_MAX, VISCOSITY_STEP)
        );
    }

    private void initWindBar(View v){
        windBar = v.findViewById(R.id.windStrengthBar);
        windValueText = v.findViewById(R.id.windStrengthValueText);

        windBar.setOnSeekBarChangeListener(
                new SliderBarListener(windBar, windValueText, WIND_MIN, WIND_MAX, WIND_STEP)
        );
    }

    private void initResolutionSpinner(View v) {
        resolutionSpinner = v.findViewById(R.id.resolutionSpinner);


        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(),
                R.layout.spinner_item,
                ResolutionItems.stringValues()
        );

        // Set to this resource layout to remove radio button from menu
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        resolutionSpinner.setAdapter(adapter);
        resolutionSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
                ResolutionItems res = ResolutionItems.values()[pos];

                switch(res){
                    case LOW:
                        updateResolution(6);
                        break;
                    case HIGH:
                        updateResolution(24);
                        break;
                    default:
                        updateResolution(12);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
    }

    private void initColorPicker(View v){
        backgroundColor = v.findViewById(R.id.backgroundColorPicker);
        backgroundColor.addOnColorChangedListener(new OnColorChangedListener() {
            @Override
            public void onColorChanged(int i) {
                int blue = (i>>0)&0xFF;
                int green = (i>>8)&0xFF;
                int red = (i>>16)&0xFF;
                updateBackgroundColor((red)/255.0f,(green)/255.0f,(blue)/255.0f);
            }
        });

        filterColor = v.findViewById(R.id.filterColorPicker);
        filterColor.addOnColorChangedListener(new OnColorChangedListener() {
            @Override
            public void onColorChanged(int i) {
                int blue = (i>>0)&0xFF;
                int green = (i>>8)&0xFF;
                int red = (i>>16)&0xFF;
                updateFilterColor((red)/255.0f,(green)/255.0f,(blue)/255.0f);
            }
        });
    }

    private void initColorSpacePicker(View v){
        colorSpaceX = v.findViewById(R.id.colorSpaceXPicker);
        colorSpaceY = v.findViewById(R.id.colorSpaceYPicker);
        colorSpaceZ = v.findViewById(R.id.colorSpaceZPicker);

        colorSpaceX.setMinValue(1);
        colorSpaceY.setMinValue(1);
        colorSpaceZ.setMinValue(1);
        colorSpaceX.setMaxValue(100);
        colorSpaceY.setMaxValue(100);
        colorSpaceZ.setMaxValue(100);

        colorSpaceX.setWrapSelectorWheel(false);
        colorSpaceY.setWrapSelectorWheel(false);
        colorSpaceZ.setWrapSelectorWheel(false);
        colorSpaceX.setFormatter(formatter);
        colorSpaceY.setFormatter(formatter);
        colorSpaceZ.setFormatter(formatter);

        colorSpaceX.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker numberPicker, int i, int i1) {
                updateColorSpace(i1/10.0f, colorSpaceY.getValue()/10.0f, colorSpaceZ.getValue()/10.0f);
            }
        });

        colorSpaceY.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker numberPicker, int i, int i1) {
                updateColorSpace(colorSpaceX.getValue()/10.0f, i1/10.0f, colorSpaceZ.getValue()/10.0f);
            }
        });

        colorSpaceZ.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
            @Override
            public void onValueChange(NumberPicker numberPicker, int i, int i1) {
                updateColorSpace(colorSpaceX.getValue()/10.0f, colorSpaceY.getValue()/10.0f, i1/10.0f);
            }
        });

    }

    NumberPicker.Formatter formatter = new NumberPicker.Formatter(){
        @Override
        public String format(int i) {
            DecimalFormat df = new DecimalFormat("#0.00");
            return df.format((float)(i)/10.0f);
        }
    };

    private static class SliderBarListener implements SeekBar.OnSeekBarChangeListener{

        private SeekBar seekBar;
        private TextView seekBarText;
        private final int minValue;
        private final int maxValue;
        private final float step;
        private int value;

        public SliderBarListener(SeekBar seekBar, TextView seekBarText, int minValue, int maxValue, float step){
            this.minValue = minValue;
            this.maxValue = maxValue;
            this.step = step;
            this.seekBar = seekBar;
            this.seekBarText = seekBarText;
            seekBar.setMax(maxValue);

            seekBarText.setText(String.valueOf(minValue));
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            if(i < minValue){
                i = minValue;
            }
            value = i;
            seekBarText.setText(String.valueOf(round(value*step, 2)));
        }

        private static double round(double value, int places) {
            if (places < 0) throw new IllegalArgumentException();

            BigDecimal bd = new BigDecimal(Double.toString(value));
            bd = bd.setScale(places, RoundingMode.HALF_UP);
            return bd.doubleValue();
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            switch (seekBar.getId()){
                case R.id.windStrengthBar:
                    updateWind(value*step);
                    break;
                case R.id.vorticityBar:
                    updateVorticity(value*step);
                    break;
                case R.id.buoyancyBar:
                    updateBuoyancy(value*step);
                    break;
                case R.id.projectionIterationsBar:
                    updateIterations((int)(value*step));
                    break;
                case R.id.smokeKinematicViscosityBar:
                    updateViscosity(value*step);
                    break;
                case R.id.resolutionSpinner:
                    break;
            }
        }

        public native void updateWind(float strength);
        public native void updateVorticity(float vorticityScale);
        public native void updateBuoyancy(float buoyancyScale);
        public native void updateIterations(int iterations);
        public native void updateViscosity(float viscosity);
    }

    public native void updateResolution(int lowerRes);
    public native void updateBackgroundColor(float red, float green, float blue);
    public native void updateFilterColor(float red, float green, float blue);
    public native void updateColorSpace(float X, float Y, float Z);
}
