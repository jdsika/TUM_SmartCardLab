clear

folder = 'traces-';
traces = zeros(0, 125000);
ciphertext = zeros(0,16);
plaintext = zeros(0,16);

for k = 0:11
    currentFolder = [folder, num2str(k), filesep];
    display(['Loading ', currentFolder, ' ...']);

    load([currentFolder, 'trace_matrix.mat']);
    currentCipher = importdata([currentFolder, 'ciphertext.csv']);
    currentPlain  = importdata([currentFolder, 'plaintext.csv']);
    
    traces = [traces; trace_matrix];
    ciphertext = [ciphertext; currentCipher];
    plaintext  = [plaintext; currentPlain];
    
    clear currentFolder currentCipher currentPlain trace_matrix
end

clear k folder