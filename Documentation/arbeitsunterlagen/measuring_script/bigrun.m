range = 0:19;

m_traces = zeros(0,0);
m_ciphertext = zeros(0,0);

for i = range
    display(['Measuring round ', num2str(i), '...']);
    outDirectory = ['traces-', num2str(i)];
    runme;

    m_traces = [m_traces; traces];
    m_ciphertext = [m_ciphertext; ciphertext];
    clear traces ciphertext
end

ciphertext = m_ciphertext;
traces = m_traces;

clear m_ciphertext m_traces range

save('measurement.mat', '-v7.3');