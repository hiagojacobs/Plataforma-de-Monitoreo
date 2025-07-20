% Configuración del puerto serial
s = serialport("COM27", 115200);
configureTerminator(s, "LF");
flush(s);

disp('Iniciando lectura y gráfica de Temperatura...');

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
        
        % Detecta si es la línea que contiene "Temperatura"
        if contains(data, "Temperatura")
            % Extrae los dos números (humedad y temperatura)
            nums = regexp(data, '([-+]?[0-9]*\.?[0-9]+)', 'match');
            
            if length(nums) >= 2
                temp_val = str2double(nums{2}); % <-- Ahora agarra el SEGUNDO número, que es temperatura
                t = toc(t0);
                
                % Guardar datos
                tiempo(end+1) = t;
                temperatura(end+1) = temp_val;
                
                % Actualizar gráfica
                set(h, 'XData', tiempo, 'YData', temperatura);
                title(sprintf('Temperatura actual: %.2f °C', temp_val));
                drawnow;
                
                % Mostrar en consola
                fprintf('Tiempo: %.2f s | Temperatura: %.2f °C\n', t, temp_val);
            end
        end
    end
    pause(0.1);
end

% Cerrar puerto al terminar
clear s;
