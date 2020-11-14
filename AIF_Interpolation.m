% Size
row=256;    col=256;
d_row=512;  d_col=512;
% Laplacian Mask
H=[0 -1 0; 0 2 0; 0 -1 0];    V=[0 0 0; -1 2 -1; 0 0 0];    DR=[-1 0 0; 0 2 0; 0 0 -1];     DL=[0 0 -1; 0 2 0; -1 0 0];
% Input Image
fin=fopen('./lena(256x256).raw','rb');   % open file
input=fread(fin,row*col,'uint8'); % read 256x256 input
fclose(fin);
% Classification 4x4 pixel Block
input = reshape(input, row, col);
direc = zeros(64);        direc = reshape(direc, [1, 4096]);    % 방향성 pre-allocation
sum_active = zeros(64);   sum_active = reshape(sum_active, [1, 4096]);  % 활동성 pre-allocation
active = zeros(64);       active = reshape(active, [1, 4096]);  % 활동성 pre-allocation
for i = 1:4:col
    for j = 1:4:row
        temp = input(i:i+3, j:j+3); % 4x4 block
        % Laplacian filter, convolution (element-wise product -> sum)
        h = abs(sum(temp(1:3, 2:4).*H, 'all')) + abs(sum(temp(1:3, 1:3).*H, 'all')) + abs(sum(temp(2:4, 1:3).*H, 'all')) + abs(sum(temp(2:4, 2:4).*H, 'all'));
        v = abs(sum(temp(1:3, 2:4).*V, 'all')) + abs(sum(temp(1:3, 1:3).*V, 'all')) + abs(sum(temp(2:4, 1:3).*V, 'all')) + abs(sum(temp(2:4, 2:4).*V, 'all'));
        dr = abs(sum(temp(1:3, 2:4).*DR, 'all')) + abs(sum(temp(1:3, 1:3).*DR, 'all')) + abs(sum(temp(2:4, 1:3).*DR, 'all')) + abs(sum(temp(2:4, 2:4).*DR, 'all'));
        dl = abs(sum(temp(1:3, 2:4).*DL, 'all')) + abs(sum(temp(1:3, 1:3).*DL, 'all')) + abs(sum(temp(2:4, 1:3).*DL, 'all')) + abs(sum(temp(2:4, 2:4).*DL, 'all'));
        sum_active(64*((i+3)/4 - 1)+(j+3)/4) = v + h + dr + dl;
        [m, idx] = max([10 h v dr dl]);
        direc(64*((i+3)/4 - 1)+(j+3)/4) = idx;  % direction = 1 x 4096
    end
end
% Activity Quantization
t = max(sum_active);
for i = 1:4096
    active(i) = fix( sum_active(i)/(round(t(1)/5)+1) ) * 5; % active = 1 x 4096
end
class = active + direc;     % class = 1 x 4096  
% Ground Truth Image
fin=fopen('./lena(512x512).raw','rb');      % open file
gt_imgbuf=fread(fin, d_row*d_col,'uint8');  % read 512x512 ground truth
fclose(fin);
gt = reshape(gt_imgbuf, d_row, d_col);      % 1d -> 2d 512x512
input_d = imresize(input, 2, 'box');          % 256 -> 512 double input
% SixTab
input_p = padarray(input_d, [4 4], 'replicate');        % Create Padded Array
S = [1 -5 20 20 -5 1]/32;
for i = 1:8:d_col
    for j = 1:8:d_row
        for k = 1:2:8   % 가로
            temp = input_p(i+k+3, [j j+2 j+4 j+6 j+8 j+10]);
            input_p(i+k+3, j+5) = temp*S';
        end
        for l = 1:8     % 세로
            temp = input_p(j+l+3, [i i+2 i+4 i+6 i+8 i+10]);
            input_p(j+l+3, i+5) = temp*S';
        end
    end
end
result_six = input_p(5:516, 5:516);
% Filter Optimization
wc = zeros(20);     wc_h = reshape(wc, [4, 100]); wc_v = reshape(wc, [4, 100]);
for i = 1:8:d_col
    for j = 1:8:d_row
        for k = 1:2:8
            x_v = input_d((i+k-1), [j j+2 j+4 j+6]);  
            x_h = input_d((j+k-1), [i i+2 i+4 i+6]);      % 1x4 block X 
            y_v = gt((i+k-1), [j+1 j+3 j+5 j+7]);
            y_h = gt((j+k-1), [i+1 i+3 i+5 i+7]);       % 1x4 block Y
            wtemp_v = pinv((x_v.'*x_v))*(x_v.'*y_v);      % wc_vertical
            wtemp_h = pinv((x_h.'*x_h))*(x_h.'*y_h);      % wc_horizontal
            c = class(64*((i+7)/8 - 1)+(j+7)/8) - 1;
            wc_v(1:4, 1+4*c:4+4*c) = wc_v(1:4, 1+4*c:4+4*c) + wtemp_v;
            wc_h(1:4, 1+4*c:4+4*c) = wc_h(1:4, 1+4*c:4+4*c) + wtemp_h;
        end
    end
end
for i = 1:25
    q = sum(class==i)*4;
    wc_v(1:4, 1+4*(i-1):4+4*(i-1)) = (wc_v(1:4, 1+4*(i-1):4+4*(i-1))/q);
    wc_h(1:4, 1+4*(i-1):4+4*(i-1)) = (wc_h(1:4, 1+4*(i-1):4+4*(i-1))/q);
end
% Adaptive
result = input_d;
for i = 1:8:d_col
    for j = 1:8:d_row
        c = class(64*((i+7)/8 - 1)+(j+7)/8) - 1;
        for k = 1:2:8   % 가로
            temp = input_d(i+k-1, [j j+2 j+4 j+6]);
            result(i+k-1, [j+1 j+3 j+5 j+7]) = temp*wc_v(1:4, 1+4*c:4+4*c);
        end
        for l = 1:8     % 세로
            temp = result(j+l-1, [i i+2 i+4 i+6]);
            result(j+l-1, [i+1 i+3 i+5 i+7]) = temp*wc_h(1:4, 1+4*c:4+4*c);
        end
    end
end
% Filter Selection
err = zeros(5); err_w = reshape(err, [1 25]);   err_s = reshape(err, [1 25]);
for i = 1:8:d_col
    for j = 1:8:d_row
        c = class(64*((i+7)/8 - 1)+(j+7)/8);
        err_w(c) = err_w(c) + sum((result(i:i+7, j:j+7) - gt(i:i+7, j:j+7)).^2, 'all');
        err_s(c) = err_s(c) + sum((result_six(i:i+7, j:j+7) - gt(i:i+7, j:j+7)).^2, 'all');
    end
end
fout=fopen('./AIF_lena(512x512).raw', 'wb');
for i = 1:8:d_col
    for j = 1:8:d_row
        c = class(64*((i+7)/8 - 1)+(j+7)/8);
        if err_w(c) > err_s(c)
            result(i:i+7, j:j+7) = result_six(i:i+7, j:j+7);
        end
    end
end
fwrite(fout, result);
fclose(fout);
% RMS, PSNR
sse = sum((result - gt).^2, 'all'); mse = sse / (d_col * d_row);
psnr = 20 * log10(255 / sqrt(mse)); rms = sqrt(mse);
disp(rms);
disp(psnr);

