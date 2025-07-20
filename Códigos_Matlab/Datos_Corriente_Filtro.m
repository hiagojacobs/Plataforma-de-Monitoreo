clc;
clear;
close all;

% Configura el puerto serial
puerto = serialport("COM27", 115200);
configureTerminator(puerto, "LF");
flush(puerto);

% Inicialización
numMaxPuntos = 100;
corrienteCruda = nan(1, numMaxPuntos);
corrienteFiltrada = nan(1, numMaxPuntos);
tiempo = (0:numMaxPuntos - 1);
tiempo0 = tic;

% Crear la figura
figure('Name', 'Corriente Cruda vs Filtrada', 'NumberTitle', 'off');
hCruda = animatedline('Color', 'r', 'LineWidth', 1.5, 'DisplayName', 'Cruda');
hFiltrada = animatedline('Color', 'b', 'LineWidth', 1.5, 'DisplayName', 'Filtrada');
legend('show');
grid on;
xlabel('Tiempo (s)', 'FontSize', 12);
ylabel('Corriente (A)', 'FontSize', 12);
title('Corriente medida por ACS758 (Cruda vs Filtrada)', 'FontSize', 14, 'FontWeight', 'bold');
ylim([-10 10]);
xlim([0 30]);
hold on;

disp('Esperando datos del Arduino...');

while true
    try
        if puerto.NumBytesAvailable > 0
            linea = readline(puerto);
            linea = strrep(linea, ',', '.');  % Reemplaza coma por punto decimal
            linea = strtrim(linea);
            disp(['Recibido: ', linea]);  % Mostrar la línea recibida

            % Separar los dos valores por tabulación o espacio
            partes = split(linea, '\t');
            if numel(partes) ~= 2
                partes = split(linea);  % intento alternativo si no hay tab
            end

            cruda = str2double(partes{1});
            filtrada = str2double(partes{2});
            t = toc(tiempo0);

            if ~isnan(cruda) && ~isnan(filtrada)
                % Agrega al gráfico
                addpoints(hCruda, t, cruda);
                addpoints(hFiltrada, t, filtrada);

                xlim([max(0, t - 30), t]);  % desliza la ventana de tiempo
                drawnow limitrate;
            else
                warning('Datos inválidos: %s', linea);
            end
        end
    catch ME
        warning('Error leyendo datos: %s', ME.message);
    end
end
