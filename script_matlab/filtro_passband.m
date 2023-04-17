Kb = 0.07295965727;
Kf = 0.7071067812;
b0 = Kb;
b2 = [1 0 -1]; 
a2 = [1 -2*Kf*(1-Kb) 1-2*Kb];

b = b0*b2;
a = a2;
[h0,w0] =freqz(b,a,512,16000);

Kf = 0;
b0 = Kb;
b2 = [1 0 -1]; 
a2 = [1 -2*Kf*(1-Kb) 1-2*Kb];
b = b0*b2;
a = a2;
[h1,w1] = freqz(b,a,512,16000);

Kf = -0.7071067812;
b0 = Kb;
b2 = [1 0 -1]; 
a2 = [1 -2*Kf*(1-Kb) 1-2*Kb];
b = b0*b2;
a = a2;
[h2,w2] = freqz(b,a,512,16000);

figure(1)
hold on
plot(w0,20*log10(abs(h0)))
plot(w1,20*log10(abs(h1)))
plot(w2,20*log10(abs(h2)))

ax = gca;
grid on;
ax.YLim = [-30 1];
ax.XLim = [0 8000];
ax.XTick = 0:500:8000;
xlabel('Frequência')
ylabel('Magnitude (dB)')
lh = legend("fo=2000","fo=4000","fo=6000");
title("Resposta em Magnitude do filtro IIR de segunda ordem")

figure(2)
hold on
grid on
plot(w0,(angle(h0)))
plot(w1,(angle(h1)))
plot(w2,(angle(h2)))



ax = gca;
grid on;
ax.YLim = [-2 2];
ax.XLim = [0 8000];
ax.XTick = 0:500:8000;
xlabel('Frequência')
ylabel('fase (rad)')
lh = legend("fo=2000","fo=4000","fo=6000");
title("Resposta em fase do filtro IIR de segunda ordem")











Kb1 = 0.07295965727;
Kf1 = 1;
bb1 = Kb1;
bb2 = [1 0 -1]; 
aa1 = [1 -2*Kf1*(1-Kb1) 1-2*Kb1];

bb = bb1*bb2;
aa = aa1;

[h0,w0] =freqz(bb,aa,512,16000);


figure(3)
hold on
plot(w0,20*log10(abs(h0)))
plot(w0,(angle(h0)))

ax = gca;
grid on;
ax.YLim = [-30 1];
ax.XLim = [0 8000];
ax.XTick = 0:500:8000;
xlabel('Frequência')
ylabel('Magnitude (dB)')
lh = legend("magnitude", "fase");
title("Resposta em fase do filtro IIR de segunda ordem fo = 0")













Kb1 = 0.07295965727;
Kf1 = -1;
bb1 = Kb1;
bb2 = [1 0 -1]; 
aa1 = [1 -2*Kf1*(1-Kb1) 1-2*Kb1];

bb = bb1*bb2;
aa = aa1;

[h0,w0] =freqz(bb,aa,512,16000);


figure(4)
hold on
plot(w0,20*log10(abs(h0)))
plot(w0,(angle(h0)))

ax = gca;
grid on;
ax.YLim = [-30 4];
ax.XLim = [0 8000];
ax.XTick = 0:500:8000;
xlabel('Frequência')
ylabel('Magnitude (dB)')
lh = legend("magnitude", "fase");
title("Resposta em fase do filtro IIR de segunda ordem fo = fs/2")


