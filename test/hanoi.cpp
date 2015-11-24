//#include <stdio.h>
//#include <Windows.h>
//
//void move(char x, char y) {
//	printf("%c¡ª¡ª>%c\n", x, y);
//}
//
//void hanoi(int n, char one, char two, char three) {
//	if (n == 1) {
//		move(one, three);
//	}
//	else {
//		hanoi(n - 1, one, three, two);
//		move(one, three);
//		hanoi(n - 1, two, one, three);
//	}
//}
//
//int main() {
//	int m;
//	printf("Input the number of dishes:");
//	scanf("%d", &m);
//	printf("The step to moving %d dishes:\n", m);
//	hanoi(m, 'A', 'B', 'C');
//
//	system("pause");
//	return 0;
//}