% Configuración de la comunicación serial
s = serialport("COM27", 115200);
configureTerminator(s, "LF");
flush(s);

disp('Iniciando lectura y gráfica de temperatura...');

% Variables para almacenar datos
tiempo = [];
temperatura = [];

% Configuración de la gráfica
figure;
h = plot(nan, nan, '-o');
xlabel('Tiempo (s)');
ylabel('Temperatura (°C)');
title('Lectura de Temperatura en Tiempo Real');
grid on;
hold on;

% Tiempo inicial
t0 = tic;

% Bucle de lectura continua
while true
    if s.NumBytesAvailable > 0
        data = readline(s);
        
        if contains(data, "Temperatura")
            temp = regexp(data, '([-+]?[0-9]*\.?[0-9]+)', 'match');
            if ~isempty(temp)
                temp_val = str2double(temp{1});
                t = toc(t0);
                
                % Guardar datos
                tiempo(end+1) = t;
                temperatura(end+1) = temp_val;
                
                % Actualizar gráfica
                set(h, 'XData', tiempo, 'YData', temperatura);
                drawnow;
                
                fprintf('Tiempo: %.2f s - Temperatura: %.2f °C\n', t, temp_val);
            end
        end
    end
    pause(0.1);
end

% Recuerda cerrar la conexión cuando termines
clear s;