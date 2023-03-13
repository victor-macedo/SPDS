alfa = [0.996,0.962,0.718];
n= 3


b = [1-alfa(n)];
a = [1 -alfa(n)];

figure(1)
subplot(1,1,1)
zplane(b,a)
hold on
