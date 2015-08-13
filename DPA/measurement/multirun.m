%% Runs Multiple Smartcard Measurements
range = 0:24;

nTraces = 200;              % Number of traces measured
sampleRate = 125e6;         % Minimum sample rate used [S/s]
sampleTime = 1.5e-3;        % Total sampled time per measurement [s]

for i = range
    display(['Measuring round ', num2str(i), '...']);
    outDirectory = '/tmp/clone-dummy-1_5k-2';
    smartcard_measurement(nTraces,sampleRate,sampleTime,outDirectory,i);
end
