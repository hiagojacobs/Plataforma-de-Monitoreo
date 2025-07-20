clc;
clear;
close all;

% Configurar puerto serial
puerto = serialport("COM27", 115200);     % Asegurate que COM27 es correcto
configureTerminator(puerto, "LF");
flush(puerto);

% Inicialización
numMaxPuntos = 200;
voltajeCrudo = nan(1, numMaxPuntos);
voltajeFiltrado = nan(1, numMaxPuntos);
tiempo = nan(1, numMaxPuntos);
tiempo0 = tic;

% Crear figura
figure('Name', 'Voltaje Crudo vs Filtrado', 'NumberTitle', 'off');
hCrudo = animatedline('Color', 'r', 'LineWidth', 1.5, 'DisplayName', 'Crudo');
hFiltrado = animatedline('Color', 'b', 'LineWidth', 1.5, 'DisplayName', 'Filtrado');
legend('show');
grid on;
xlabel('Tiempo (s)', 'FontSize', 12);
ylabel('Voltaje (V)', 'FontSize', 12);
title('Voltaje Crudo vs Filtrado', 'FontSize', 14, 'FontWeight', 'bold');
ylim([0 20]); % Ajusta según tu rango de batería
xlim([0 30]);
hold on;

disp('Esperando datos desde Arduino...');

while true
    try
        if puerto.NumBytesAvailable > 0
            linea = readline(puerto);
            linea = strrep(linea, ',', '.');  % por si decimal viene con coma
            linea = strtrim(linea);
            disp(['Recibido: ', linea]);

            % Separar por tabulación o espacio
            partes = split(linea, '\t');
            if numel(partes) ~= 2
                partes = split(linea);  % fallback si no hay \t
            end

            crudo = str2double(partes{1});
            filtrado = str2double(partes{2});
            t = toc(tiempo0);

            if ~isnan(crudo) && ~isnan(filtrado)
                % Añadir datos al gráfico
                addpoints(hCrudo, t, crudo);
                addpoints(hFiltrado, t, filtrado);

                xlim([max(0, t - 30), t]);  % Ventana deslizante de tiempo
                drawnow limitrate;
            else
                warning('Datos inválidos: %s', linea);
            end
        end
    catch ME
        warning('Error leyendo datos: %s', ME.message);
    end
end
