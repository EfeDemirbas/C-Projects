close all;
clear all;
clc;


gamma = 1.4;
M = linspace(0,2,11);
p_ratio = (1+(gamma-1)/2*M.^2).^(1/(gamma-1));

plot(M,p_ratio)