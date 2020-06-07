package com.pbf;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.NumberPicker;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.ToggleButton;

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

    // Resolution Scale
    private static final int RESOLUTION_SCALE_MIN = 2;
    private static final int RESOLUTION_SCALE_MAX = 40;
    private static final float RESOLUTION_SCALE_STEP = 0.5f;
    // Simulation Scale
    private static final int SIMULATION_SCALE_MIN = 1;
    private static final int SIMULATION_SCALE_MAX = 30*2;
    private static final float SIMULATION_SCALE_STEP = 0.5f;
    // Time Step
    private static final int TIME_STEP_MIN = 10;
    private static final int TIME_STEP_MAX = 120;
    private static final float TIME_STEP_STEP = 1.0f;
    // Object Radius
    private static final int OBJECT_RADIUS_MIN = 1;
    private static final int OBJECT_RADIUS_MAX = 30;
    private static final float OBJECT_RADIUS_STEP = 1.0f;
    // Object Temperature
    private static final int OBJECT_TEMPERATURE_MIN = 0;
    private static final int OBJECT_TEMPERATURE_MAX = 200;
    private static final float OBJECT_TEMPERATURE_STEP = 25.0f;
    // Object Density
    private static final int OBJECT_DENSITY_MIN = 0;
    private static final int OBJECT_DENSITY_MAX = 100;
    private static final float OBJECT_DENSITY_STEP = 0.01f;
    // Object Velocity
    private static final int OBJECT_VELOCITY_MIN = 0;
    private static final int OBJECT_VELOCITY_MAX = 100;
    private static final float OBJECT_VELOCITY_STEP = 0.1f;
    // Wind Strength
    private static final int WIND_STRENGTH_MIN = 0;
    private static final int WIND_STRENGTH_MAX = 300;
    private static final float WIND_STRENGTH_STEP = 0.25f;
    // Wind ANGLE
    private static final int WIND_ANGLE_MIN = 0;
    private static final int WIND_ANGLE_MAX = 360;
    private static final float WIND_ANGLE_STEP = 1.0f;
    // Vorticity
    private static final int VORTICITY_MIN = 0;
    private static final int VORTICITY_MAX = 1000;
    private static final float VORTICITY_STEP = 0.05f;
    // Buoyancy
    private static final int BUOYANCY_MIN = 0;
    private static final int BUOYANCY_MAX = 100;
    private static final float BUOYANCY_STEP = 0.01f;
    // Smoke Dissipation
    private static final int SMOKE_DISSIPATION_MIN = 0;
    private static final int SMOKE_DISSIPATION_MAX = 100;
    private static final float SMOKE_DISSIPATION_STEP = 0.01f;
    // Temperature Viscosity
    private static final int TEMPERATURE_VISCOSITY_MIN = 0;
    private static final int TEMPERATURE_VISCOSITY_MAX = 100;
    private static final float TEMPERATURE_VISCOSITY_STEP = 0.01f;
    // Smoke Viscosity
    private static final int SMOKE_VISCOSITY_MIN = 0;
    private static final int SMOKE_VISCOSITY_MAX = 100;
    private static final float SMOKE_VISCOSITY_STEP = 0.01f;
    // Velocity Viscosity
    private static final int VELOCITY_VISCOSITY_MIN = 0;
    private static final int VELOCITY_VISCOSITY_MAX = 100;
    private static final float VELOCITY_VISCOSITY_STEP = 0.01f;
    // Min Noise Band
    private static final int MIN_NOISE_BAND_MIN = 1;
    private static final int MIN_NOISE_BAND_MAX = 100;
    private static final float MIN_NOISE_BAND_STEP = 0.1f;
    // Max Noise Band
    private static final int MAX_NOISE_BAND_MIN = 1;
    private static final int MAX_NOISE_BAND_MAX = 100;
    private static final float MAX_NOISE_BAND_STEP = 0.1f;
    // Density Diffusion Iterations
    private static final int DENSITY_DIFFUSION_ITERATIONS_MIN = 1;
    private static final int DENSITY_DIFFUSION_ITERATIONS_MAX = 100;
    private static final float DENSITY_DIFFUSION_ITERATIONS_STEP = 1.0f;
    // Velocity Diffusion Iterations
    private static final int VELOCITY_DIFFUSION_ITERATIONS_MIN = 1;
    private static final int VELOCITY_DIFFUSION_ITERATIONS_MAX = 100;
    private static final float VELOCITY_DIFFUSION_ITERATIONS_STEP = 1.0f;
    // Projection Iterations
    private static final int PROJECTION_ITERATIONS_MIN = 1;
    private static final int PROJECTION_ITERATIONS_MAX = 100;
    private static final float PROJECTION_ITERATIONS_STEP = 1.0f;

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



    private enum ObjectTypeItems{
        SPHERE, CUBE, PYRAMID, CYLINDER, CONE, FLOOR, WALL, DUALSPHERES;

        public static String[] stringValues(){
            ObjectTypeItems[] objectTypeItems = ObjectTypeItems.values();
            String[] values = new String[objectTypeItems.length];
            for(int i = 0; i < objectTypeItems.length; i++){
                values[i] = objectTypeItems[i].name();
            }
            return values;
        }
    };

    private enum BoundariesItems{
        NONE, SOME;

        public static String[] stringValues(){
            BoundariesItems[] boundariesItems = BoundariesItems.values();
            String[] values = new String[boundariesItems.length];
            for(int i = 0; i < boundariesItems.length; i++){
                values[i] = boundariesItems[i].name();
            }
            return values;
        }
    };

    private NumberPicker colorSpaceX, colorSpaceY, colorSpaceZ;

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.settings_fragment, container, false);
        initUIElements(view);
        return view;
    }

    private void initUIElements(View v){
        initTouchModeButton(v);
        initOrientationButton(v);
        initResolutionSpinner(v);
        initResolutionScaleBar(v);
        initSimulationScaleBar(v);
        initTimeStepBar(v);
        initColorPicker(v);
        initColorSpacePicker(v);
        initObjectTypeSpinner(v);
        initObjectRadiusBar(v);
        initObjectTemperatureBar(v);
        initObjectDensityBar(v);
        initObjectVelocityBar(v);
        initWindStrengthBar(v);
        initWindAngleBar(v);
        initVorticityBar(v);
        initBuoyancyBar(v);
        initSmokeDissipationBar(v);
        initTemperatureViscosityBar(v);
        initSmokeViscosityBar(v);
        initVelocityViscosityBar(v);
        initMinNoiseBandBar(v);
        initMaxNoiseBandBar(v);
        initDensityDiffusionIterationsBar(v);
        initVelocityDiffusionIterationsBar(v);
        initProjectionIterationsBar(v);
        initBoundariesSpinner(v);

    }

    private void initTouchModeButton(View v){
        ToggleButton touchModeButton = v.findViewById(R.id.touch);
        touchModeButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                setTouchMode(isChecked);
            }
        });
    }

    private void initOrientationButton(View v){
        ToggleButton orientationButton = v.findViewById(R.id.orientation);
        orientationButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                setOrientation(isChecked);
            }
        });
    }

    private void initResolutionSpinner(View v) {
        Spinner resolutionSpinner = v.findViewById(R.id.resolutionSpinner);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(),
                R.layout.spinner_item,
                ResolutionItems.stringValues()
        );

        // Set to this resource layout to remove radio button from menu
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        resolutionSpinner.setAdapter(adapter);
        resolutionSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            boolean startup = true;
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
                if (startup){
                    startup = false;
                    return;
                }

                ResolutionItems res = ResolutionItems.values()[pos];

                switch(res){
                    case LOW:
                        updateResolution(6);
                        break;
                    case HIGH:
                        updateResolution(24);
                        break;
                    case MEDIUM:
                        updateResolution(12);
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        // Todo: get from settings instead
        resolutionSpinner.setSelection(1);
    }

    private void initResolutionScaleBar(View v){
        SeekBar resolutionScaleBar = v.findViewById(R.id.resolutionScaleBar);
        TextView resolutionScaleValueText = v.findViewById(R.id.resolutionScaleValueText);

        resolutionScaleBar.setOnSeekBarChangeListener(
                new SliderBarListener(resolutionScaleBar, resolutionScaleValueText, RESOLUTION_SCALE_MIN, RESOLUTION_SCALE_MAX, RESOLUTION_SCALE_STEP)
        );
        // Todo: get from settings instead
        resolutionScaleBar.setProgress((int)(5.0f/RESOLUTION_SCALE_STEP));
    }

    private void initSimulationScaleBar(View v){
        SeekBar simulationScaleBar = v.findViewById(R.id.simulationScaleBar);
        TextView simulationScaleValueText = v.findViewById(R.id.simulationScaleValueText);

        simulationScaleBar.setOnSeekBarChangeListener(
                new SliderBarListener(simulationScaleBar, simulationScaleValueText, SIMULATION_SCALE_MIN, SIMULATION_SCALE_MAX, SIMULATION_SCALE_STEP)
        );
        // Todo: get from settings instead
        simulationScaleBar.setProgress((int)(24.0f/SIMULATION_SCALE_STEP));
    }

    private void initTimeStepBar(View v){
        SeekBar timeStepBar = v.findViewById(R.id.timeStepBar);
        TextView timeStepValueText = v.findViewById(R.id.timeStepValueText);

        timeStepBar.setOnSeekBarChangeListener(
                new SliderBarListener(timeStepBar, timeStepValueText, TIME_STEP_MIN, TIME_STEP_MAX, TIME_STEP_STEP)
        );
        // Todo: get from settings instead
        timeStepBar.setProgress((int)(30.0f/TIME_STEP_STEP));
    }

    private void initColorPicker(View v){
        ColorPickerView backgroundColor = v.findViewById(R.id.backgroundColorPicker);
        backgroundColor.addOnColorChangedListener(new OnColorChangedListener() {
            @Override
            public void onColorChanged(int i) {
                int blue = (i>>0)&0xFF;
                int green = (i>>8)&0xFF;
                int red = (i>>16)&0xFF;
                updateBackgroundColor((red)/255.0f,(green)/255.0f,(blue)/255.0f);
            }
        });

        // Todo: get from settings instead
        backgroundColor.setLightness(0);

        ColorPickerView filterColor = v.findViewById(R.id.filterColorPicker);
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

        // Todo: get from settings instead
        colorSpaceX.setValue(18);
        colorSpaceY.setValue(22);
        colorSpaceZ.setValue(22);

    }

    private void initObjectTypeSpinner(View v) {
        Spinner objectTypeSpinner = v.findViewById(R.id.objectTypeSpinner);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(),
                R.layout.spinner_item,
                ObjectTypeItems.stringValues()
        );

        // Set to this resource layout to remove radio button from menu
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        objectTypeSpinner.setAdapter(adapter);
        objectTypeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            boolean startup = true;
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
                if (startup){
                    startup = false;
                    return;
                }

                ObjectTypeItems res = ObjectTypeItems.values()[pos];

                switch(res){
                    case SPHERE:
                        updateObjectType("SPHERE");
                        break;
                    case CUBE:
                        updateObjectType("CUBE");
                        break;
                    case PYRAMID:
                        updateObjectType("PYRAMID");
                        break;
                    case CYLINDER:
                        updateObjectType("CYLINDER");
                        break;
                    case CONE:
                        updateObjectType("CONE");
                        break;
                    case FLOOR:
                        updateObjectType("FLOOR");
                        break;
                    case WALL:
                        updateObjectType("WALL");
                        break;
                    case DUALSPHERES:
                        updateObjectType("DUALSPHERES");
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
    }

    private void initObjectRadiusBar(View v){
        SeekBar objectRadiusBar = v.findViewById(R.id.objectRadiusBar);
        TextView objectRadiusValueText = v.findViewById(R.id.objectRadiusValueText);

        objectRadiusBar.setOnSeekBarChangeListener(
                new SliderBarListener(objectRadiusBar, objectRadiusValueText, OBJECT_RADIUS_MIN, OBJECT_RADIUS_MAX, OBJECT_RADIUS_STEP)
        );
        // Todo: get from settings instead
        objectRadiusBar.setProgress((int)(8.0f/OBJECT_RADIUS_STEP));
    }

    private void initObjectTemperatureBar(View v){
        SeekBar objectTemperatureBar = v.findViewById(R.id.objectTemperatureBar);
        TextView objectTemperatureValueText = v.findViewById(R.id.objectTemperatureValueText);

        objectTemperatureBar.setOnSeekBarChangeListener(
                new SliderBarListener(objectTemperatureBar, objectTemperatureValueText, OBJECT_TEMPERATURE_MIN, OBJECT_TEMPERATURE_MAX, OBJECT_TEMPERATURE_STEP)
        );
        // Todo: get from settings instead
        objectTemperatureBar.setProgress((int)(3500.0f/OBJECT_TEMPERATURE_STEP));
    }

    private void initObjectDensityBar(View v){
        SeekBar objectDensityBar = v.findViewById(R.id.objectDensityBar);
        TextView objectDensityValueText = v.findViewById(R.id.objectDensityValueText);

        objectDensityBar.setOnSeekBarChangeListener(
                new SliderBarListener(objectDensityBar, objectDensityValueText, OBJECT_DENSITY_MIN, OBJECT_DENSITY_MAX, OBJECT_DENSITY_STEP)
        );
        // Todo: get from settings instead
        objectDensityBar.setProgress((int)(0.4f/OBJECT_DENSITY_STEP));
    }

    private void initObjectVelocityBar(View v){
        SeekBar objectVelocityBar = v.findViewById(R.id.objectVelocityBar);
        TextView objectVelocityValueText = v.findViewById(R.id.objectVelocityValueText);

        objectVelocityBar.setOnSeekBarChangeListener(
                new SliderBarListener(objectVelocityBar, objectVelocityValueText, OBJECT_VELOCITY_MIN, OBJECT_VELOCITY_MAX, OBJECT_VELOCITY_STEP)
        );
    }

    private void initWindStrengthBar(View v){
        SeekBar windStrengthBar = v.findViewById(R.id.windStrengthBar);
        TextView windStrengthValueText = v.findViewById(R.id.windStrengthValueText);

        windStrengthBar.setOnSeekBarChangeListener(
                new SliderBarListener(windStrengthBar, windStrengthValueText, WIND_STRENGTH_MIN, WIND_STRENGTH_MAX, WIND_STRENGTH_STEP)
        );
    }

    private void initWindAngleBar(View v){
        SeekBar windAngleBar = v.findViewById(R.id.windAngleBar);
        TextView windAngleText = v.findViewById(R.id.windAngleValueText);
        CheckBox windAngleCheckBox = v.findViewById(R.id.windAngleCheckBox);

        windAngleBar.setOnSeekBarChangeListener(
                new SliderBarListener(windAngleBar, windAngleText, WIND_ANGLE_MIN, WIND_ANGLE_MAX, WIND_ANGLE_STEP)
        );

        windAngleCheckBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                setWindAngle(isChecked);
            }
        });
    }

    private void initVorticityBar(View v){
        SeekBar vorticityBar = v.findViewById(R.id.vorticityBar);
        TextView vorticityValueText = v.findViewById(R.id.vorticityValueText);

        vorticityBar.setOnSeekBarChangeListener(
                new SliderBarListener(vorticityBar, vorticityValueText, VORTICITY_MIN, VORTICITY_MAX, VORTICITY_STEP)
        );
        // Todo: get from settings instead
        vorticityBar.setProgress((int)(8.0f/VORTICITY_STEP));
    }

    private void initBuoyancyBar(View v){
        SeekBar buoyancyBar = v.findViewById(R.id.buoyancyBar);
        TextView buoyancyValueText = v.findViewById(R.id.buoyancyValueText);

        buoyancyBar.setOnSeekBarChangeListener(
                new SliderBarListener(buoyancyBar, buoyancyValueText, BUOYANCY_MIN, BUOYANCY_MAX, BUOYANCY_STEP)
        );
        // Todo: get from settings instead
        buoyancyBar.setProgress((int)(0.15f/BUOYANCY_STEP));
    }

    private void initSmokeDissipationBar(View v){
        SeekBar smokeDissipationBar = v.findViewById(R.id.smokeDissipationBar);
        TextView smokeDissipationValueText = v.findViewById(R.id.smokeDissipationValueText);

        smokeDissipationBar.setOnSeekBarChangeListener(
                new SliderBarListener(smokeDissipationBar, smokeDissipationValueText, SMOKE_DISSIPATION_MIN, SMOKE_DISSIPATION_MAX, SMOKE_DISSIPATION_STEP)
        );
    }

    private void initTemperatureViscosityBar(View v){
        SeekBar temperatureViscosityBar = v.findViewById(R.id.temperatureKinematicViscosityBar);
        TextView temperatureViscosityValueText = v.findViewById(R.id.temperatureKinematicViscosityValueText);

        temperatureViscosityBar.setOnSeekBarChangeListener(
                new SliderBarListener(temperatureViscosityBar, temperatureViscosityValueText, TEMPERATURE_VISCOSITY_MIN, TEMPERATURE_VISCOSITY_MAX, TEMPERATURE_VISCOSITY_STEP)
        );
    }

    private void initSmokeViscosityBar(View v){
        SeekBar smokeViscosityBar = v.findViewById(R.id.smokeKinematicViscosityBar);
        TextView smokeViscosityValueText = v.findViewById(R.id.smokeKinematicViscosityValueText);

        smokeViscosityBar.setOnSeekBarChangeListener(
                new SliderBarListener(smokeViscosityBar, smokeViscosityValueText, SMOKE_VISCOSITY_MIN, SMOKE_VISCOSITY_MAX, SMOKE_VISCOSITY_STEP)
        );
    }

    private void initVelocityViscosityBar(View v){
        SeekBar velocityViscosityBar = v.findViewById(R.id.velocityKinematicViscosityBar);
        TextView velocityViscosityValueText = v.findViewById(R.id.velocityKinematicViscosityValueText);

        velocityViscosityBar.setOnSeekBarChangeListener(
                new SliderBarListener(velocityViscosityBar, velocityViscosityValueText, VELOCITY_VISCOSITY_MIN, VELOCITY_VISCOSITY_MAX, VELOCITY_VISCOSITY_STEP)
        );
    }

    private void initMinNoiseBandBar(View v){
        SeekBar minNoiseBandBar = v.findViewById(R.id.bandMinBar);
        TextView minNoiseBandText = v.findViewById(R.id.bandMinValueText);
        CheckBox minNoiseBandCheckBox = v.findViewById(R.id.bandMinCheckBox);

        minNoiseBandBar.setOnSeekBarChangeListener(
                new SliderBarListener(minNoiseBandBar, minNoiseBandText, MIN_NOISE_BAND_MIN, MIN_NOISE_BAND_MAX, MIN_NOISE_BAND_STEP)
        );

        minNoiseBandCheckBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                setMinNoiseBand(isChecked);
            }
        });

        // Todo: get from settings instead
        minNoiseBandBar.setProgress((int)(2.0f/MIN_NOISE_BAND_STEP));
    }

    private void initMaxNoiseBandBar(View v){
        SeekBar maxNoiseBandBar = v.findViewById(R.id.bandMaxBar);
        TextView maxNoiseBandText = v.findViewById(R.id.bandMaxValueText);
        CheckBox maxNoiseBandCheckBox = v.findViewById(R.id.bandMaxCheckBox);

        maxNoiseBandBar.setOnSeekBarChangeListener(
                new SliderBarListener(maxNoiseBandBar, maxNoiseBandText, MAX_NOISE_BAND_MIN, MAX_NOISE_BAND_MAX, MAX_NOISE_BAND_STEP)
        );

        maxNoiseBandCheckBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                setMaxNoiseBand(isChecked);
            }
        });

        // Todo: get from settings instead
        maxNoiseBandBar.setProgress((int)(8.0f/MAX_NOISE_BAND_STEP));
    }

    private void initDensityDiffusionIterationsBar(View v){
        SeekBar densityDiffusionIterationsBar = v.findViewById(R.id.densityDiffusionIterationsBar);
        TextView densityDiffusionIterationsValueText = v.findViewById(R.id.densityDiffusionIterationsValueText);

        densityDiffusionIterationsBar.setOnSeekBarChangeListener(
               new SliderBarListener(densityDiffusionIterationsBar, densityDiffusionIterationsValueText, DENSITY_DIFFUSION_ITERATIONS_MIN, DENSITY_DIFFUSION_ITERATIONS_MAX, DENSITY_DIFFUSION_ITERATIONS_STEP)
       );
    }

    private void initVelocityDiffusionIterationsBar(View v){
        SeekBar velocityDiffusionIterationsBar = v.findViewById(R.id.velocityDiffusionIterationsBar);
        TextView velocityDiffusionIterationsValueText = v.findViewById(R.id.velocityDiffusionIterationsValueText);

        velocityDiffusionIterationsBar.setOnSeekBarChangeListener(
                new SliderBarListener(velocityDiffusionIterationsBar, velocityDiffusionIterationsValueText, VELOCITY_DIFFUSION_ITERATIONS_MIN, VELOCITY_DIFFUSION_ITERATIONS_MAX, VELOCITY_DIFFUSION_ITERATIONS_STEP)
        );
    }

    private void initProjectionIterationsBar(View v){
        SeekBar projectionIterationsBar = v.findViewById(R.id.projectionIterationsBar);
        TextView projectionIterationsValueText = v.findViewById(R.id.projectionIterationsValueText);

        projectionIterationsBar.setOnSeekBarChangeListener(
                new SliderBarListener(projectionIterationsBar, projectionIterationsValueText, PROJECTION_ITERATIONS_MIN, PROJECTION_ITERATIONS_MAX, PROJECTION_ITERATIONS_STEP)
        );
        // Todo: get from settings instead
        projectionIterationsBar.setProgress((int)(20.0f/PROJECTION_ITERATIONS_STEP));
    }

    private void initBoundariesSpinner(View v) {
        Spinner boundariesSpinner = v.findViewById(R.id.boundariesSpinner);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(),
                R.layout.spinner_item,
                BoundariesItems.stringValues()
        );

        // Set to this resource layout to remove radio button from menu
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        boundariesSpinner.setAdapter(adapter);
        boundariesSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            boolean startup = true;
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
                if (startup){
                    startup = false;
                    return;
                }

                BoundariesItems res = BoundariesItems.values()[pos];

                switch(res){
                    case NONE:
                        updateBoundaries("NONE");
                        break;
                    case SOME:
                        updateBoundaries("SOME");
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

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
                case R.id.resolutionScaleBar:
                    updateResolutionScale(value*step);
                    break;
                case R.id.simulationScaleBar:
                    updateSimulationScale(value*step);
                    break;
                case R.id.timeStepBar:
                    updateTimeStep(value*step);
                    break;
                case R.id.objectRadiusBar:
                    updateObjectRadius(value*step);
                    break;
                case R.id.objectTemperatureBar:
                    updateObjectTemperature(value*step);
                    break;
                case R.id.objectDensityBar:
                    updateObjectDensity(value*step);
                    break;
                case R.id.objectVelocityBar:
                    updateObjectVelocity(value*step);
                    break;
                case R.id.windStrengthBar:
                    updateWindStrength(value*step);
                    break;
                case R.id.windAngleBar:
                    updateWindAngle(value*step);
                    break;
                case R.id.vorticityBar:
                    updateVorticity(value*step);
                    break;
                case R.id.buoyancyBar:
                    updateBuoyancy(value*step);
                    break;
                case R.id.smokeDissipationBar:
                    updateSmokeDissipation(value*step);
                    break;
                case R.id.temperatureKinematicViscosityBar:
                    updateTemperatureViscosity(value*step);
                    break;
                case R.id.smokeKinematicViscosityBar:
                    updateSmokeViscosity(value*step);
                    break;
                case R.id.velocityKinematicViscosityBar:
                    updateVelocityViscosity(value*step);
                    break;
                case R.id.bandMinBar:
                    updateMinNoiseBand(value*step);
                    break;
                case R.id.bandMaxBar:
                    updateMaxNoiseBand(value*step);
                    break;
                case R.id.densityDiffusionIterationsBar:
                    updateDensityDiffusionIterations((int)(value*step));
                    break;
                case R.id.velocityDiffusionIterationsBar:
                    updateVelocityDiffusionIterations((int)(value*step));
                    break;
                case R.id.projectionIterationsBar:
                    updateProjectionIterations((int)(value*step));
                    break;

            }
        }
        public native void updateResolutionScale(float scale);
        public native void updateSimulationScale(float scale);
        public native void updateTimeStep(float time_step);
        public native void updateObjectRadius(float radius);
        public native void updateObjectTemperature(float temperature);
        public native void updateObjectDensity(float density);
        public native void updateObjectVelocity(float velocity);
        public native void updateWindStrength(float strength);
        public native void updateWindAngle(float angle);
        public native void updateVorticity(float vorticityScale);
        public native void updateBuoyancy(float buoyancyScale);
        public native void updateSmokeDissipation(float dissipation);
        public native void updateTemperatureViscosity(float viscosity);
        public native void updateSmokeViscosity(float viscosity);
        public native void updateVelocityViscosity(float viscosity);
        public native void updateMinNoiseBand(float band);
        public native void updateMaxNoiseBand(float band);
        public native void updateDensityDiffusionIterations(int iterations);
        public native void updateVelocityDiffusionIterations(int iterations);
        public native void updateProjectionIterations(int iterations);
    }

    public native void setTouchMode(boolean touchMode);
    public native void setOrientation(boolean orientationMode);
    public native void updateResolution(int lowerRes);
    public native void updateBackgroundColor(float red, float green, float blue);
    public native void updateFilterColor(float red, float green, float blue);
    public native void updateColorSpace(float X, float Y, float Z);
    public native void updateObjectType(String type);
    public native void setWindAngle(boolean custom);
    public native void setMinNoiseBand(boolean custom);
    public native void setMaxNoiseBand(boolean custom);
    public native void updateBoundaries(String mode);
}
