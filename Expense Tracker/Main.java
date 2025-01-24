package main_code;
import java.util.*;

public class Main {
    public static void main(String[] args) {
        ExpenseTracker tracker = new ExpenseTracker();
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\nExpense Tracker Menu:");
            System.out.println("1. Add Expense");
            System.out.println("2. Remove Expense");
            System.out.println("3. View Expenses");
            System.out.println("4. Total Expenses");
            System.out.println("5. Exit");
            System.out.print("Enter your choice (1-5): ");

            int choice = scanner.nextInt();
            scanner.nextLine(); // Consume newline

            switch (choice) {
                case 1:
                    System.out.print("Enter the date (YYYY-MM-DD): ");
                    String date = scanner.nextLine();
                    System.out.print("Enter the description: ");
                    String description = scanner.nextLine();
                    System.out.print("Enter the amount: ");
                    double amount = scanner.nextDouble();
                    Expense expense = new Expense(date, description, amount);
                    tracker.addExpense(expense);
                    System.out.println("Expense added successfully.");
                    break;

                case 2:
                    System.out.print("Enter the expense index to remove: ");
                    int index = scanner.nextInt() - 1;
                    tracker.removeExpense(index);
                    break;

                case 3:
                    tracker.viewExpenses();
                    break;

                case 4:
                    tracker.totalExpenses();
                    break;

                case 5:
                    System.out.println("Have a nice day!");
                    scanner.close();
                    return;

                default:
                    System.out.println("Invalid choice, please try again.");
            }
        }
    }
}
