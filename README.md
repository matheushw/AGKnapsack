# AGKnapsack

Nesse projeto estudamos a aplicação de algoritmos genéticos no problema Knapsack (ou o Problema da Mochila). Apesar de existirem soluções exatas para o mesmo, elas não são eficientes para instâncias grandes do problema.

Existem ainda aproximações gulosas das soluções do problema, as quais são muitas vezes extremamente próximas ao ótimo global. Assim, o problema Knapsack é ideal para a comparação de diferentes variações de Algoritmos Evolutivos, já que é possível testar rapidamente diferentes combinações de parâmetros.

## O problema

O Problema da Mochila consiste no seguinte processo de otimização:

Dada uma mochila de capacidade $k$ e $n$ objetos, cada um associado a um par $(p_i, v_i)$ que representa o seu Peso e seu Valor, escolha um subconjunto de objetos de modo a maximizar o seu valor total sujeito à condição de que o seu peso total é menor ou igual à capacidade da mochila.

É possível representando uma possível solução com um vetor binário $[c_i]$, com cada entrada $c_i$ indicando se o elemento $i$ está ou não no subconjunto escolhido. Dessa maneira, o problema se resume a:

Encontre 
$$\max \sum_i c_i v_i$$
Sujeito à condição: 
$$\sum_i c_i p_i \leq k$$

## As soluções exatas

Para podermos comparar diferentes variedades do algoritmo evolutivo, é necessário conhecer o verdadeiro valor ótimo para a otimização anterior. São utilizados dois algoritmos para comparação:
 - Uma solução ótima por programação dinâmica em $O(nk)$
 - Uma solução gulosa, próxima da ótima, em $O(N)$

## O Indivíduo

Cada indivídio representa uma tentativa de solução do problema, consistindo de um cromossomo que é um vetor booleano.

Como na solução ótima a maioria dos itens não são utilizados, os indivíduos são inicializados em 0.


## A Medida de Fitness

Caso um indivíduo tenha um cromossomo com um peso total válido (isto é, menor que a capacidade da mochila), o seu fitness é igual ao valor total dos objetos.

Porém, para indivíduos com pesos inválidos, não é possível utilizar esse valor. Em seu lugar, usamos o seu peso total multiplicado por $-1$, para levar o Algoritmo Evolutivo a preferir cromossomos válidos.


## O Crossover

Já que a ordem dos objetos no vetor é completamente arbitrária, não faz sentido a utilização de métodos tradicionais de Crossover como a divisão do cromossomo de cada um dos pais ao meio.

Em vez disso, o crossover é feito posição à posição, sendo a escolha de cada gene do filho independente:
![[crossover.jpg]]


## A Mutação

A mutação consiste na troca do valor de um único gene do cromossomo. A taxa de mutação corresponde à chance de tal troca ocorrer.

Nós realizamos experimentos tanto com taxas de mutação constante e com taxas adaptativas.


## A Seleção

A seleção dos indíviduos para reprodução pode ser feita de três maneiras distintas:

 - Elitismo, no qual o melhor indivíduo é encontrado e todos os outros realizam o crossover com ele
 - Torneio de 2, no qual são realizados sucessivos torneios
 - Roleta, na qual é feita uma seleção aleatória, com probabilidades proporcionais ao fitness de cada indivíduo

Em todos os casos, é garantido que o melhor indivíduo será preservado para a próxima geração.



## A Predação

Como o número de gerações é geralmente grande, a população acaba se tornando relativamente homogênea, então decidimos utilizar a Predação em alguns dos experimentos, o que consiste em periodicamente trocar o pior indivíduo da população por um indivíduo novo.

O novo indivíduo é sempre incializado em 0, como explicado acima.


## Experimentos

Abaixo estão os resultados de diversos experimentos realizados:
Onde o mesmo não estiver especificado, o algoritmo utilizado será a Seleção por Roleta com Mutação Variável e Predação, com uma população de 100 indivíduos, sendo realizadas 1000 gerações.


### Efeitos do tamanho do problema
É esperado que, ao aumentar o possível número de itens possíveis, ocorra uma redução na performance do Algoritmo Evolutivo comparado à solução ótima, devido ao grande aumento do espaço de busca. Para confirmar essa hipótese, foram feitos sucessivos experimentos com diferentes valores de $n$ (escala logarítmica no eixo x):

![[scale.png]]

Assim, vemos que a solução obtida é quase ótima para $n < 100$, deteriorando em seguida devido ao aumento do espaço de busca.


### Efeitos do tamanho da população e número de gerações
O custo computacional do Algoritmo Evolutivo é aproximadamente $O(P \cdot G)$, onde $P$ é o tamanho da população e $G$ é o número de gerações. Assim, é possível variar um dos valores arbitrariamente, e o custo computacional será aproximadamente o mesmo, desde que o outro valor seja alterado para manter o produto constante.

Para verificar essa hipótese, nós realizamos experimentos, fixando $P \cdot G$ em $100000$, na tentativa de encontrar os valores ótimos para o tamanho da população e o número de gerações em um dado tempo (o tamanho do cromossomo é fixado em $1000$):

![[population.png]]


Assim, nós vemos que o algoritmo funciona melhor com populações relativamente grandes, as quais permitem uma maior exploração do espaço de busca, mesmo com a redução no número de gerações.


### Efeitos do método de seleção
Nós realizamos experimentos comparando os diferentes métodos de seleção (torneio de 2, elitismo e roleta) para diferentes tamanhos do problema. Os resultados estão resumidos no gráfico abaixo:

![[selection.png]]

Assim, vemos que o elitismo se destacou sobre os demais.



### Efeitos da Predação
Em nossos experimentos, percebemos que para o problema da mochila a diferença entre a performance com e sem a predação foi extremamente pequena, provavelmente devido ao tamanho considerável da população e do número de gerações, diluindo o efeito da predação.

### Efeitos da mutação variável
Como a mutação ocorre em apenas um único gene, utilizar a mutação variável não teve alterações significativas na performance.



## Curvas de treinamento
Abaixo estão algumas curvas de treinamento, comparadas às soluções ótima e gulosa. Pode ser visto que em alguns casos, o Algoritmo Evolutivo chega próximo da solução ótima. Porém, em outros casos, o mesmo fica preso em um mínimo local no início do treinamento.

![[runs.png]]



