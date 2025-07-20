% ---------------------------------------------
% Lectura y graficado de voltaje desde Arduino (estilo profesional)
% ---------------------------------------------

% Configuración del puerto serial
puerto = serialport("COM27", 115200);  % Cambiar COM27 si es necesario
configureTerminator(puerto, "LF");
flush(puerto); % Limpia el buffer de entrada

% Parámetros de adquisición
numLecturas = 100;
voltajes = nan(1, numLecturas);
tiempo = (0:numLecturas - 1);
dt = 0.5;  % Intervalo de muestreo (segundos)
t = tiempo * dt;

% Crear figura y gráfico
figure('Name', 'Voltaje en Tiempo Real', 'NumberTitle', 'off');
hPlot = plot(t, voltajes, '-o', 'LineWidth', 1.5, 'MarkerSize', 6);
grid on;
xlabel('Tiempo (s)', 'FontSize', 12);
ylabel('Voltaje (V)', 'FontSize', 12);
title('Lectura de Voltaje en Tiempo Real', 'FontSize', 14, 'FontWeight', 'bold');

% Ajuste del eje Y
ylim([11 12]);         % Zoom entre 11V y 12V
yticks(11:0.1:12);     % Marcas decimales detalladas

drawnow; % Asegura que la ventana se renderice

% Inicio de lectura
disp('Esperando datos desde Arduino...');

while true
    try
        if puerto.NumBytesAvailable > 0
            linea = readline(puerto);
            voltaje = str2double(strtrim(linea));

            if ~isnan(voltaje)
                voltajes = [voltajes(2:end), voltaje];
                set(hPlot, 'YData', voltajes);
                drawnow limitrate;
            end
        end
    catch ME
        warning('Error leyendo datos: %s', ME.message);
    end
end
