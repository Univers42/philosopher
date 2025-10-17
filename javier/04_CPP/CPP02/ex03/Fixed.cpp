/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 17:52:44 by jrollon-          #+#    #+#             */
/*   Updated: 2025/07/07 12:10:16 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fixed.hpp"

/*	NOTAS DE FIXED-POINT NUMERO (ha costado pero ahi va):
	El problema con los float o double es que son numeros que en representacion
	binaria se hace mediante el sistema IEEE 754 que determina que se puede
	representar un número float con 4 bytes (32 bits) o un Double con 8 bytes
	(64bits). Tomando como ejemplo los Float, entonces por IEEE754 seria así su
	fórmula interna harcodeada para ser "traducida" a ese numero float.

	s | eeeeeeee | mmmmmmmmmmmmmmmmmmmmmmm
	1bit  8bits       23bits

	De tal forma que la fórmula es: 

	s -> el bit más de la izquierda, si es 1 es un número negativo. Si es 0 es positivo
	
	e -> 8bits que representa el exponente en binario 2^exponente. Por lo que al ser
		8bits podria ser hasta 256 dicho exponente, pero NO. Porque:
			00000000 -> se reserva para el 0.xxxx como número float. si no sería HARDCODEADO un 1.xxxx como float (se lo añaden por la cara)
			11111111 y 00000000000000000000000 (la mantisa) -> seria +- infinito dependiendo del signo (algo / 0)
			11111111 y mantisa diferente de todo 0 -> NaN (no un número = 0 / 0, ó sqrt(-1) ó infinito - infinito)
		Y el resto de opciones tiene un desplazamiento a -127. De tal forma que:
			00000001 (1) = 1 - 127 = 126 de exponente (2^(-126))
			01111111 (127) = 127 - 127 = 0 de exponente (2^(0) = 1)
			11111110 (254) = 254 - 127 = 127 de exponente (2^127) 
			00000000 (0) = en este caso es especial y comparte el exponente como -126 pero la parte de la mantisa será 0 en vez de 1.

	m -> es la mantisa (lo que va antes del 2^expo -> 1.mmm * 2^exponente). Y NO es una representacion en decimal del numero con 23 bits.
		Cada bit es un 2^-posicion que ocupa asi:
			10000000000000000000000 -> 2^-1 = 0.5
			00000000000000000000001 -> 2^-23 = 0.000000119
			10000000000000000000001 -> 2^-1 + 2^-23 = 0.500000119
	
	ENTONCES para representar un numero en float seria esta la formula:
	
		 (−1)^sign× (1+fracción de mantisa) × 2^exponente real - si e es distinto de 00000000 o 11111111
		 (−1)^sign× (0+fracción de mantisa) × 2^exponente real - si e es 00000000

		1.0 = 0 01111111 00000000000000000000000 -> (-1)^0 * (1 + 0) * 2^0(por ser 127 - 127) = 1 * 1.0 * 1 = 1.0
		0 00000000 01000010000000000000010 -> (-1)^0 * (0 + 2^-2 + 2^-7 + 2^-22) * 2^-126 = 1 * (0 + 0.25 + 0.0078125 + 0.000000238) * 2^-126 = 2.901 x 10^-39
		3.5 = 1 10000000 11000000000000000000000 -> (-1)^1 * (1 + 2^-1 + 2^-2) * 2^(128-127) = -1 * (1 + 0.5 + 0.25) * 2 = 3.5 
		0 = 0 00000000 00000000000000000000000

Para Doubles la distribución es:
| Signo (1) | Exponente (11) | Mantisa (52) |

todo esto hardcodeado dentro del ordenador. Por eso va mucho más lento en rendimiento en calculos matemáticos si se hace el en float o Double, por lo que
si se quiere mejorar el rendimiento lo mejor es hacer cálculos con ENTEROS. Pero ¿como podemos representar un float como entero y no perder el valor?
Tomando la parte fracción como un número determinado de bits. Hagamoslo para 8bits y 4bits con el numero 3.5 (NOS OLVIDAMOS DEL METODO IEEE754)
8bits:
	8bits son 2^8 = 256 = 1 << 8 -> 00000001 << 8 = 1 0000 0000 
	1. Multiplicamos 3.5 x 256 = 896. ESA ES SU REPRESENTACION EN ENTERO DEL FLOAT 3.5
	2. Si queremos convertir el numero de nuevo a float solo hay que hacer el inverso -> 896 / 256 = 3.5
	Analizamos el numero 896 en binario: 00000000 00000000 00000011 10000000 (32bits)
		Si pillamos la parte fraccionaria (decimal) de 8 bits podriamos dividirlo en:
			00000000 00000000 00000011 | 10000000
					(el num 3)				(el num 128 => 128 / 256 = 0.5) = 3.5
		Si quisieramos sacar el ENTERO del numero 896 hariamos un >> 8 y moveria todo 8 bits siendo:
			00000000 00000000 00000000 00000011 que es 3. Es decir la parte entera de 3.5
4bits con 3.1
	son 2^4 = 16 =  1 << 4 -> 10000
	1. 3.1 x 16 = 49.6 pero se pilla en formato float el mas cercano al entero (roundf) = 50.0
	2. la conversion seria inversa 3.125
	como se ve hay una perdida.. por eso es mejor usar cuantos mas bits de decimal mejor.

SE PIERDE PRECISIÓN Pero se gana en rendimiento. Por eso para videojuegos y demas programas de rendimiento se hacen las operaciones con fixed-point (ints)
que son más rápidas y luego se reconvierte a float en caso de necesitarlo

OFUUUUUUUUUUUUUUU!!!
*/


/************ CANONICAL ***********/
Fixed::Fixed(void){
	//std::cout << GREEN "Default constructor called" RESET << std::endl;
	_fixedPointNum = 0;
}

Fixed::Fixed(const Fixed &other){
	//std::cout << YELLOW "Copy constructor called" RESET << std::endl;
	*this = other; 
}

/*si la copia que estamos creando es la misma direccion de memoria que nos pasan (a=a) no copiamos...*/
/*...por que aunque generalmente funcionaria sin el if, si hago liberacion de memoria dentro...*/
/*...de la funcion o asignaciones complejas a = a puede causar problemas como doble liberacion...*/
/*...corrupcion o comportamiento indefinido. Mejor proteger.*/
Fixed &Fixed::operator=(const Fixed &other){
	//std::cout << CYAN "Copy assignment operator called" RESET << std::endl;
	if (this != &other){
		_fixedPointNum = other._fixedPointNum;
	}
	return (*this);
}

Fixed::~Fixed(void){
	//std::cout << RED "Destructor called" RESET << std::endl;
}

/************ OTHER CONSTRUCTORS ***********/


Fixed::Fixed(const int num){
	//std::cout << GREEN "Int constructor called" RESET << std::endl;
	float aux = static_cast<float>(num) * (1 << _fractionBits);
	if (aux > static_cast<float>(INT_MAX) || aux < static_cast<float>(INT_MIN)){
		std::cerr << "Error: Number Overflow. It will be set to Zero." << std::endl;
		_fixedPointNum = 0;
	}
	else{
		_fixedPointNum = static_cast<int>(roundf(aux));
	}	
}

Fixed::Fixed(const float fnum){
	//std::cout << GREEN "Float constructor called" RESET << std::endl;
	float	aux = fnum * (1 << _fractionBits);
	if (aux > static_cast<float>(INT_MAX) || aux < static_cast<float>(INT_MIN)){
		std::cerr << "Error: Number Overflow. It will be set to zero" << std::endl;
		_fixedPointNum = 0;
	}
	else{
		_fixedPointNum = static_cast<int>(roundf(aux));
	}
}


/************ GETTER SETTERS ***********/

int	Fixed::getRawBits(void) const{
	//std::cout << BLUE "getRawbits member function called" RESET << std::endl;
	return (_fixedPointNum);
}

void	Fixed::setRawBits(int const raw){
	//std::cout << MAGENTA "setRawBits member function called" RESET << std::endl;
	_fixedPointNum = raw;
}

/************ MEMBER FUNCTIONS ***********/

float	Fixed::toFloat(void) const{
	return (static_cast<float>(_fixedPointNum) / (1 << _fractionBits)); // /256 = 2^8
}
int		Fixed::toInt(void) const{
	return (static_cast<int>(_fixedPointNum >> _fractionBits));
}

Fixed 		&Fixed::min(Fixed &lFixed, Fixed &rFixed){
	if (lFixed._fixedPointNum <= rFixed._fixedPointNum){
		return (lFixed);
	}
	return (rFixed);
}
const Fixed &Fixed::min(const Fixed &lFixed, const Fixed &rFixed){
	if (lFixed._fixedPointNum <= rFixed._fixedPointNum){
		return (lFixed);
	}
	return (rFixed);
}
Fixed 		&Fixed::max(Fixed &lFixed, Fixed &rFixed){
	if (lFixed._fixedPointNum >= rFixed._fixedPointNum){
		return (lFixed);
	}
	return (rFixed);
}
const Fixed &Fixed::max(const Fixed &lFixed, const Fixed &rFixed){
	if (lFixed._fixedPointNum >= rFixed._fixedPointNum){
		return (lFixed);
	}
	return (rFixed);
}

/************ MEMBER OVERLOAD OPERATORS ***********/
bool	Fixed::operator>(const Fixed &rightFixed) const{
	if (this->_fixedPointNum > rightFixed._fixedPointNum){
		return (true);
	}
	return (false);
}

bool	Fixed::operator<(const Fixed &rightFixed) const{
	return(this->_fixedPointNum < rightFixed._fixedPointNum);
}

bool	Fixed::operator>=(const Fixed &rightFixed) const{
	return(this->_fixedPointNum >= rightFixed._fixedPointNum);
}

bool	Fixed::operator<=(const Fixed &rightFixed) const{
	return(this->_fixedPointNum <= rightFixed._fixedPointNum);
}

bool	Fixed::operator==(const Fixed &rightFixed) const{
	return(this->_fixedPointNum == rightFixed._fixedPointNum);
}

bool	Fixed::operator!=(const Fixed &rightFixed) const{
	return(this->_fixedPointNum != rightFixed._fixedPointNum);
}

Fixed	Fixed::operator+(const Fixed &rightFixed) const{
	Fixed		aux;
	long long	auxNum = static_cast<long long>(this->_fixedPointNum) + rightFixed._fixedPointNum;
	
	if (auxNum > INT_MAX || auxNum < INT_MIN){
		std::cerr << "Error: Number Overflow in addition. It will set to cero" << std::endl;
		aux._fixedPointNum = 0;
	}
	else{
		aux._fixedPointNum = static_cast<int>(auxNum);
	}
	return (aux);
}

Fixed	Fixed::operator-(const Fixed &rightFixed) const{
	Fixed		aux;
	long long	auxNum = static_cast<long long>(this->_fixedPointNum) - rightFixed._fixedPointNum;
	
	if (auxNum > INT_MAX || auxNum < INT_MIN){
		std::cerr << "Error: Number Overflow in substraction. It will set to cero" << std::endl;
		aux._fixedPointNum = 0;
	}
	else{
		aux._fixedPointNum = static_cast<int>(auxNum);
	}
	return (aux);
}

Fixed	Fixed::operator*(const Fixed &rightFixed) const{
	Fixed		aux;
	long long	auxNum = static_cast<long long>(this->_fixedPointNum) * rightFixed._fixedPointNum;
	
	if (auxNum > INT_MAX || auxNum < INT_MIN){
		std::cerr << "Error: Number Overflow in addition. It will set to zero" << std::endl;
		aux._fixedPointNum = 0;
	}
	else{
		aux._fixedPointNum = static_cast<int>((auxNum) >> _fractionBits); // divide por 256 porque...
		//...al multiplicar dos números en fixed point (cada uno ya multiplicado por 256)...
		//...el resultado queda multiplicado por 256², no por 256¹. Para mantener la escala...
		//...correcta de fixed point, dividimos por 256 para volver a la escala original.
	}
	return (aux);
}

Fixed	Fixed::operator/(const Fixed &rightFixed) const{
	Fixed	aux;
	
	if (rightFixed._fixedPointNum == 0){
		if (this->_fixedPointNum > 0){
			std::cout << "Warning: The fixedPoint is infinite. It will be reset to zero" << std::endl;
		}
		else if (this->_fixedPointNum < 0){
			std::cout << "Warning: The fixedPoint is minus infinite. It will be reset to zero" << std::endl;
		}
		else if (this->_fixedPointNum == 0){
			std::cout << "Warning: The fixedPoint NaN. It will be reset to zero" << std::endl;
		}
		aux._fixedPointNum = 0;
	}
	else{ 
		//aux = Fixed (this->toFloat() / rightFixed.toFloat()); //se ha de hacer conversion a float porque...
		//...al haber convertido el numero a fixedPoint lo multiplicamos por 256. Si ahora dividimos...
		//...esos dos 256 en cada division se anulan y por lo tanto quedan como si fuera una división...
		//...de los enteros o floats del que venian, perdiendo datos. Habria que convertir a float dividir...
		//...y que el float resultante se convertiria a fixed_point no perdiendo asi datos....
		//...pero si se pasa a float se pierde eficiencia, por lo que se multiplica por 256 (<< _fractionBits)...
		//...y asi el resultado sigue siendo una multiplicacion por 256 y no se anulan.
		long long	auxNum = (static_cast<long long>(this->_fixedPointNum) << _fractionBits) / rightFixed._fixedPointNum;	
		if (auxNum > INT_MAX || auxNum < INT_MIN){
			std::cerr << "Error division overflow. It will be set to zero" << std::endl;
			aux._fixedPointNum = 0;
		}
		else{
			aux._fixedPointNum = static_cast<int>(auxNum);
		}
	}
	return (aux);
}

Fixed	&Fixed::operator++(void){
	long long	auxNum;
	
	auxNum = static_cast<long long>(this->_fixedPointNum) + 1;
	if (auxNum > INT_MAX || auxNum < INT_MIN){
		std::cerr << "Error: ++Fixed is overflow" << std::endl;
	}
	else{
		this->_fixedPointNum = static_cast<int>(auxNum);
	}
	return (*this);
}

Fixed	Fixed::operator++(int){
	long long	auxNum;
	Fixed		temp = *this;
	
	auxNum = static_cast<long long>(this->_fixedPointNum) + 1;
	if (auxNum > INT_MAX || auxNum < INT_MIN)
	{
		std::cerr << "Error: Fixed++ is overflow" << std::endl;
	}
	else{
		this->_fixedPointNum = static_cast<int>(auxNum);
	}
	return (temp); //debe devolver el valor anterior por que el incremento se hace despues.
	//...de todas formas queda cambiado en this->_fixedPointNum b = a++, b recibe el valor anterior (temp)
}

Fixed	&Fixed::operator--(void){
	long long	auxNum;
	
	auxNum = static_cast<long long>(this->_fixedPointNum) - 1;
	if (auxNum > INT_MAX || auxNum < INT_MIN){
		std::cerr << "Error: --Fixed is overflow" << std::endl;
	}
	else{
		this->_fixedPointNum = static_cast<int>(auxNum);
	}
	return (*this);
}

Fixed	Fixed::operator--(int){
	long long	auxNum;
	Fixed		temp = *this;
	
	auxNum = static_cast<long long>(this->_fixedPointNum) - 1;
	if (auxNum > INT_MAX || auxNum < INT_MIN)
	{
		std::cerr << "Error: Fixed-- is overflow" << std::endl;
	}
	else{
		this->_fixedPointNum = static_cast<int>(auxNum);
	}
	return (temp); //b = a--, be recibe el valor anterior de a y a se actualiza.
} 


/************ OTHER FUNCTIONS ***********/

std::ostream &operator<<(std::ostream &os, const Fixed &num){
	os << num.toFloat();
	return(os);
}

