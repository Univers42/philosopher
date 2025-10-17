/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_math.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 14:51:57 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/17 14:57:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdbool.h>

bool    within_range(unsigned short min, unsigned short max, unsigned short v)
{
    return ((v - min) <= (max - min));   
} 