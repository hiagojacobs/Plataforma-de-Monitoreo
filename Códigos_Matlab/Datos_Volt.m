% ---------------------------------------------
% Lectura y graficado de voltaje en tiempo real (compatible con salida cruda del Arduino)
% ---------------------------------------------

% Configuración del puerto serial
puerto = serialport("COM27", 115200);  % Cambiar COM27 si corresponde
configureTerminator(puerto, "LF");
flush(puerto); % Limpia el buffer

% Parámetros de adquisición
numLecturas = 100;
voltajes = nan(1, numLecturas);
tiempo = (0:numLecturas - 1);
dt = 0.5;  % intervalo en segundos (coincide con delay del Arduino)
t = tiempo * dt;

% Crear figura y gráfico
figure('Name', 'Voltaje en Tiempo Real', 'NumberTitle', 'off');
hPlot = plot(t, voltajes, '-o', 'LineWidth', 1.5, 'MarkerSize', 6);
grid on;
xlabel('Tiempo (s)', 'FontSize', 12);
ylabel('Voltaje (V)', 'FontSize', 12);
title('Lectura de Voltaje desde Arduino', 'FontSize', 14, 'FontWeight', 'bold');
ylim([0 20]);

drawnow; % Forzar que aparezca la ventana

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

                % Autoajuste del eje Y si es necesario
                yLimits = ylim(gca);
                if voltaje < yLimits(1) || voltaje > yLimits(2)
                    ylim([min(voltajes)-1, max(voltajes)+1]);
                end

                drawnow limitrate;
            end
        end
    catch ME
        warning('Error leyendo datos: %s', ME.message);
    end
end
