clc;                             
clear;                            
close all;                       

puerto = serialport("COM27", 115200);          % Abre el puerto serie COM27 con baudrate 115200
configureTerminator(puerto, "LF");             % Configura el terminador de línea como "LF" (Line Feed = salto de línea)
flush(puerto);                                 % Limpia el búfer de entrada/salida del puerto

numMaxPuntos = 100;                            % Cantidad máxima de puntos a mostrar en el gráfico
corriente = nan(1, numMaxPuntos);              % Inicializa vector de corriente con valores NaN
tiempo = (0:numMaxPuntos - 1);                 % Inicializa el eje X (tiempo) con valores iniciales
tiempo0 = tic;                                 % Inicia un cronómetro para contar el tiempo transcurrido

figure('Name', 'Corriente en Tiempo Real', 'NumberTitle', 'off'); % Crea una figura con nombre personalizado
hPlot = plot(tiempo, corriente, '-o', ...      % Crea el objeto de gráfico y lo guarda en hPlot
    'LineWidth', 1.5, 'Color', 'b');           % Configura el estilo de línea (azul, grosor 1.5)

grid on;                                       % Activa la cuadrícula en el gráfico
xlabel('Tiempo (s)', 'FontSize', 12);          % Etiqueta del eje X
ylabel('Corriente (A)', 'FontSize', 12);       % Etiqueta del eje Y
title('Corriente medida por ACS758', ...       % Título del gráfico
    'FontSize', 14, 'FontWeight', 'bold');
ylim([-10 10]);                                % Limita el eje Y entre -10 y 10 A
drawnow;                                       % Fuerza a dibujar la figura en pantalla

disp('Esperando datos del Arduino...');        % Muestra mensaje informativo

while true                                     % Bucle infinito para leer y graficar continuamente
    try
        if puerto.NumBytesAvailable > 0              % Verifica si hay datos disponibles en el puerto
            linea = readline(puerto);                % Lee una línea del puerto serie
            linea = strrep(linea, ',', '.');         % Reemplaza comas por puntos (por si viene decimal con coma)
            linea = strtrim(linea);                  % Elimina espacios en blanco al inicio y fin
            disp(['Recibido: ', linea]);             % Muestra el dato recibido

            valor = str2double(linea);               % Convierte el string a número decimal

            if ~isnan(valor)                         
                t = toc(tiempo0);                    % Obtiene el tiempo actual desde tiempo0
                corriente = [corriente(2:end), valor]; % Desplaza y actualiza vector de corriente
                tiempo = [tiempo(2:end), t];           % Desplaza y actualiza vector de tiempo

                set(hPlot, 'XData', tiempo, 'YData', corriente); 
                xlim([max(0, t - 30), t]);            % Muestra últimos 30 segundos en el eje X
                drawnow limitrate;                    % Dibuja la figura limitando la tasa de refresco
            else
                warning('Dato inválido: "%s"', linea); 
            end
        end
    catch ME
        warning('Error leyendo datos: %s', ME.message); 
    end
end
