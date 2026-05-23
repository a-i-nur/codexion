/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 21:41:01 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/05/23 23:47:26 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_str_to_num(const char *num)
{
	int		i;
	long	res;

	if (!num || num[0] == '\0')
		return (-1);
	i = 0;
	res = 0;
	while (num[i] == '0')
		i++;
	if (i > 0)
		i--;
	while (num[i])
	{
		if (num[i] < '0' || num[i] > '9')
			return (-1);
		res = res * 10;
		res = res + (num[i] - '0');
		if (res > INT_MAX)
			return (-1);
		i++;
	}
	return ((int)res);
}

size_t	ft_strlen(const char *str)
{
	size_t	counter;

	counter = 0;
	while (str[counter] != '\0')
		counter++;
	return (counter);
}

int	ft_strncmp(const char *str1, const char *str2, size_t num)
{
	const unsigned char	*strng1;
	const unsigned char	*strng2;
	size_t				i;

	strng1 = (const unsigned char *)str1;
	strng2 = (const unsigned char *)str2;
	i = 0;
	while (i < num)
	{
		if (strng1[i] != strng2[i] || strng1[i] == '\0' || strng2[i] == '\0')
			return (strng1[i] - strng2[i]);
		i++;
	}
	return (0);
}
