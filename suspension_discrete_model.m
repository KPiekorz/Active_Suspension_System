clc;
clear all;
close all;

m1 = 2500;
m2 = 320;
k1 = 80000;
k2 = 500000;
b1 = 350;
b2 = 15020;

% X=[ X1
%     X1_dot
%     Y1
%     Y1_dot
%     Y2];

% U = [ Force
%       Road];

Ac=[0                 1   0                                              0         0
   -(b1*b2)/(m1*m2)   0   ((b1/m1)*((b1/m1)+(b1/m2)+(b2/m2)))-(k1/m1)   -(b1/m1)   0
    b2/m2             0  -((b1/m1)+(b1/m2)+(b2/m2))                      1         0
    k2/m2             0  -((k1/m1)+(k1/m2)+(k2/m2))                      0         0
    0                 0   1                                              0         0];

Bc=[0                 0
    1/m1              (b1*b2)/(m1*m2)
    0                -(b2/m2)
    (1/m1)+(1/m2)    -(k2/m2)
    0                 0];


Cc=[0 0 1 0 0];

Dc=[0 0];

sysc=ss(Ac,Bc,Cc,Dc);

sysd = c2d(sysc,0.1,'zoh')

[Ad, Bd, Cd, Dd, Ts] = ssdata(sysd) 

% here can be generated different types of road


x0 = transpose([0 0 0 0 5]); % initial condition
u = transpose([0 0.1]); % unit step [U-Force W-Road]

x(:,1) = Ad*x0 + Bd*u; % x(k + 1) = A*x(k) + B*u(k)

for k = 2:100
    x(:,k) = Ad*x(:,k-1) + Bd*u;
    y(k) = Cd*x(:,k);
    
    % here can be implemented controler with K matrix
    
end

figure(1);
plot(x(2, :));

figure(2);
plot(y);
